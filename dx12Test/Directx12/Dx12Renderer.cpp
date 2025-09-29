#include "pch.h"

#include "Dx12Renderer.h"
#include "Logger.h"

#include "Runtime/Dx12Runtime.h"

using namespace Microsoft::WRL;

namespace directx12
{
	Dx12Renderer::Dx12Renderer(const windows::WindowData& windowData)
		: m_windowData(windowData)
	{
		m_backBuffers.resize(m_frameCount);
		m_rtvHandles.resize(m_frameCount);
		m_commandAllocators.resize(m_frameCount);
	}

	Dx12RendererSetupResult Dx12Renderer::Setup()
	{
		using namespace directx12::runtime;

		Logger logger("Dx12Renderer");
		logger.Info("Initializing Dx12Renderer.");

		Dx12RendererSetupResult result = CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		if (result.status != Dx12ResultCode::Success)
		{
			logger.Error("Failed to create CommandQueue. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = CreateSwapChain();

		if (result.status != Dx12ResultCode::Success)
		{
			logger.Error("Failed to create SwapChain. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = CreateRTVDescriptorHeap();

		if (result.status != Dx12ResultCode::Success)
		{
			logger.Error("Failed to create RTVDescriptorheap. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = UpdateRenderTargetViews();

		if (result.status != Dx12ResultCode::Success)
		{
			logger.Error("Failed to create RenderTargetViews. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = CreateCommandAllocators();

		if (result.status != Dx12ResultCode::Success)
		{
			logger.Error("Failed to create CommandAllocators. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = CreateGraphicsCommandList();

		if (result.status != Dx12ResultCode::Success)
		{
			logger.Error("Failed to create GraphicsCommandList. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = CreateFence();

		if (result.status != Dx12ResultCode::Success)
		{
			logger.Error("Failed to create Fence. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		logger.Info("Dx12Renderer initialization complete.");
		return {};
	}

	Dx12RendererSetupResult Dx12Renderer::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		HRESULT hr = runtime::g_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateCommandQueue, Dx12ResultCode::CreateCommandQueueFailed, hr };

		return {};
	}

	Dx12RendererSetupResult Dx12Renderer::CreateSwapChain()
	{
		ComPtr<IDXGIFactory4> dxgiFactory4;
		UINT createFactoryFlags = 0;

#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4));

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateSwapChain, Dx12ResultCode::CreateDXGIFactoryFailed, hr };

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = m_windowData.clientWidth;
		desc.Height = m_windowData.clientHeight;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = FALSE;
		desc.SampleDesc = { 1, 0 };
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = m_frameCount;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = runtime::g_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		ComPtr<IDXGISwapChain1> swapChain1;
		hr = dxgiFactory4->CreateSwapChainForHwnd(
			m_commandQueue.Get(),
			m_windowData.windowHandle,
			&desc,
			nullptr,
			nullptr,
			&swapChain1
		);

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateSwapChain, Dx12ResultCode::CreateSwapChainFailed, hr };

		hr = dxgiFactory4->MakeWindowAssociation(m_windowData.windowHandle, DXGI_MWA_NO_ALT_ENTER);

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateSwapChain, Dx12ResultCode::MakeWindowAssociationFailed, hr };

		hr = swapChain1.As(&m_swapChain);

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateSwapChain, Dx12ResultCode::ComInterfaceCastFailed, hr };

		return {};
	}

	Dx12RendererSetupResult Dx12Renderer::CreateRTVDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = m_frameCount;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT hr = runtime::g_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvDescriptorHeap));

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateRTVDescriptorHeap, Dx12ResultCode::CreateDescriptorHeapFailed, hr };

		return {};
	}

	Dx12RendererSetupResult Dx12Renderer::UpdateRenderTargetViews()
	{
		UINT rtvDescriptorSize = runtime::g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		for (size_t i = 0; i < m_frameCount; ++i)
		{
			HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf()));

			if (FAILED(hr))
				return { Dx12RendererSetupContext::UpdateRenderTargetViews, Dx12ResultCode::CreateBufferFailed, hr };

			runtime::g_device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHandle);

			m_rtvHandles[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}

		return {};
	}

	Dx12RendererSetupResult Dx12Renderer::CreateCommandAllocators()
	{
		for (size_t i = 0; i < m_frameCount; i++)
		{
			HRESULT hr = runtime::g_device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(m_commandAllocators[i].GetAddressOf())
			);

			if (FAILED(hr))
				return { Dx12RendererSetupContext::CreateCommandAllocators, Dx12ResultCode::CreateCommandAllocatorFailed, hr };
		}

		return {};
	}

	Dx12RendererSetupResult Dx12Renderer::CreateGraphicsCommandList()
	{
		HRESULT hr = runtime::g_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocators[0].Get(),
			nullptr,
			IID_PPV_ARGS(&m_graphicsCommandList)
		);

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateGraphicsCommandList, Dx12ResultCode::CreateCommandListFailed, hr };

		hr = m_graphicsCommandList->Close();

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateGraphicsCommandList, Dx12ResultCode::GraphicsCommandListCloseFailed, hr };

		return {};
	}

	Dx12RendererSetupResult Dx12Renderer::CreateFence()
	{
		HRESULT hr = runtime::g_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateFence, Dx12ResultCode::CreateFenceFailed, hr };

		return {};
	}
}
