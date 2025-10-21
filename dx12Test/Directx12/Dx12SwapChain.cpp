#include "pch.h"

#include "Dx12SwapChain.h"
#include "Runtime/Dx12Runtime.h"

using namespace Microsoft::WRL;

namespace directx12
{
	Dx12SwapChain::Dx12SwapChain()
		: Dx12SwapChain(3, true)
	{
	}

	Dx12SwapChain::Dx12SwapChain(UINT bufferCount, bool vSync)
		: m_bufferCount(bufferCount), m_released(false), m_vSync(vSync),
		m_logger([]() {
		static std::atomic<uint32_t> s_swapChainInstace = 0;
		uint32_t swapChainId = s_swapChainInstace.fetch_add(1);

		return std::string("Dx12SwapChain#") + std::to_string(swapChainId);
			}())
	{
		m_backBuffers.resize(m_bufferCount);
		m_rtvHandles.resize(m_bufferCount);
	}

	Dx12SetupSwapChainResult Dx12SwapChain::Setup(const ComPtr<ID3D12CommandQueue>& commandQueue, const windows::WindowData& windowData)
	{
		assert(commandQueue);

		m_commandQueue = commandQueue;
		m_windowData = windowData;

		Dx12SetupSwapChainResult result = CreateSwapChain();

		if (result.status != Dx12ResultCode::Success)
		{
			m_logger.Error("Failed to create SwapChain. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = CreateRTVDescriptorHeap();

		if (result.status != Dx12ResultCode::Success)
		{
			m_logger.Error("Failed to create RTVDescriptorheap. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = UpdateRenderTargetViews();

		if (result.status != Dx12ResultCode::Success)
		{
			m_logger.Error("Failed to create RenderTargetViews. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		return {};
	}

	void Dx12SwapChain::Present()
	{
		UINT syncInterval = m_vSync ? 1 : 0;
		UINT PresentFlags = runtime::g_tearingSupported && !m_vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;

		ThrowIfFailed(m_swapChain->Present(syncInterval, PresentFlags));
	}

	void Dx12SwapChain::UpdateBackBufferIndex()
	{
		m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	}

	Dx12SetupSwapChainResult Dx12SwapChain::CreateSwapChain()
	{
		ComPtr<IDXGIFactory4> dxgiFactory4;
		UINT createFactoryFlags = 0;

#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4));

		if (FAILED(hr))
			return { Dx12SwapChainSetupContext::CreateSwapChain, Dx12ResultCode::CreateDXGIFactoryFailed, hr };

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = m_windowData.clientWidth;
		desc.Height = m_windowData.clientHeight;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = FALSE;
		desc.SampleDesc = { 1, 0 };
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = m_bufferCount;
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
			return { Dx12SwapChainSetupContext::CreateSwapChain, Dx12ResultCode::CreateSwapChainFailed, hr };

		hr = dxgiFactory4->MakeWindowAssociation(m_windowData.windowHandle, DXGI_MWA_NO_ALT_ENTER);

		if (FAILED(hr))
			return { Dx12SwapChainSetupContext::CreateSwapChain, Dx12ResultCode::MakeWindowAssociationFailed, hr };

		hr = swapChain1.As(&m_swapChain);

		if (FAILED(hr))
			return { Dx12SwapChainSetupContext::CreateSwapChain, Dx12ResultCode::ComInterfaceCastFailed, hr };

		return {};
	}

	Dx12SetupSwapChainResult Dx12SwapChain::CreateRTVDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = m_bufferCount;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT hr = runtime::g_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvDescriptorHeap));

		if (FAILED(hr))
			return { Dx12SwapChainSetupContext::CreateRTVDescriptorHeap, Dx12ResultCode::CreateDescriptorHeapFailed, hr };

		return {};
	}

	Dx12SetupSwapChainResult Dx12SwapChain::UpdateRenderTargetViews()
	{
		UINT rtvDescriptorSize = runtime::g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		for (size_t i = 0; i < m_bufferCount; ++i)
		{
			HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf()));

			if (FAILED(hr))
				return { Dx12SwapChainSetupContext::UpdateRenderTargetViews, Dx12ResultCode::CreateBufferFailed, hr };

			runtime::g_device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHandle);

			m_rtvHandles[i] = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}

		return {};
	}
}
