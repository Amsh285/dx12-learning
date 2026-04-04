#include "pch.h"

#include "Dx12Renderer.h"
#include "Runtime/Dx12Runtime.h"

using namespace Microsoft::WRL;

namespace directx12
{
	Logger Dx12Renderer::CreateLogger()
	{
		static std::atomic<uint32_t> counter = 0;
		return Logger("Dx12Renderer#" + std::to_string(counter++));
	}

	//Todo: Remove logger
	Dx12Renderer::Dx12Renderer()
		: m_logger(CreateLogger())
	{}

	Dx12Renderer::~Dx12Renderer()
	{
		Release();
	}

	void Dx12Renderer::Release()
	{
		m_fence.Flush();

		m_swapChain.Release();
		m_fence.Release();

		m_commandList.Reset();

		for (auto allocator : m_commandAllocators)
			allocator.Reset();

		m_commandAllocators.clear();

		m_commandQueue.Reset();
	}

	Dx12RendererSetupResult Dx12Renderer::Setup(const windows::WindowData& windowData)
	{
		using namespace directx12::runtime;

		m_commandAllocators.resize(m_frameCount);
		m_frameFenceValues.assign(m_frameCount, 0);

		m_logger.Info("Initializing Dx12Renderer.");

		Dx12RendererSetupResult result = CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		if (result.status != Dx12ResultCode::Success)
			return result;

		Dx12SetupSwapChainResult swapChainSetupResult = m_swapChain.Setup(m_commandQueue, windowData);

		if (swapChainSetupResult.status != Dx12ResultCode::Success)
			return Dx12RendererSetupResult::FromSwapChain(swapChainSetupResult.context, swapChainSetupResult.status, swapChainSetupResult.hr);

		result = CreateCommandAllocators();

		if (result.status != Dx12ResultCode::Success)
			return result;

		result = CreateGraphicsCommandList();

		if (result.status != Dx12ResultCode::Success)
			return result;

		Dx12FenceSetupResult fenceSetupResult = m_fence.Setup(m_commandQueue);

		if (fenceSetupResult.status != Dx12ResultCode::Success)
			return Dx12RendererSetupResult::FromFence(fenceSetupResult.context, fenceSetupResult.status, fenceSetupResult.hr);

		m_logger.Info("Dx12Renderer initialization complete.");
		return {};
	}

	void Dx12Renderer::Render()
	{
		UINT currentBackBufferIndex = m_swapChain.GetCurrentBackBufferIndex();
		m_fence.WaitCpu(m_frameFenceValues[currentBackBufferIndex]);

		ComPtr<ID3D12CommandAllocator> currentCommandAllocator = m_commandAllocators[currentBackBufferIndex];
		ComPtr<ID3D12Resource> currentBackBuffer = m_swapChain.GetCurrentBackBuffer();

		assert(currentCommandAllocator);
		assert(currentBackBuffer);

		ThrowIfFailed(currentCommandAllocator->Reset());
		ThrowIfFailed(m_commandList->Reset(currentCommandAllocator.Get(), nullptr));

		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = currentBackBuffer.Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			m_commandList->ResourceBarrier(1, &barrier);

			FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
			D3D12_CPU_DESCRIPTOR_HANDLE currentRtvHandle = m_swapChain.GetCurrentRTVHandle();

			m_commandList->ClearRenderTargetView(currentRtvHandle, clearColor, 0, nullptr);
		}

		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = currentBackBuffer.Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			m_commandList->ResourceBarrier(1, &barrier);
			ThrowIfFailed(m_commandList->Close());

			ID3D12CommandList* const commandLists[] = {
				m_commandList.Get()
			};

			m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
			m_swapChain.Present();
			m_frameFenceValues[currentBackBufferIndex] = m_fence.Signal();

			m_swapChain.UpdateBackBufferIndex();
		}
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
			return Dx12RendererSetupResult::FromRenderer(Dx12RendererSetupContext::CreateCommandQueue, Dx12ResultCode::CreateCommandQueueFailed, hr);

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
				return Dx12RendererSetupResult::FromRenderer(Dx12RendererSetupContext::CreateCommandAllocators, Dx12ResultCode::CreateCommandAllocatorFailed, hr);
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
			IID_PPV_ARGS(&m_commandList)
		);

		if (FAILED(hr))
			return Dx12RendererSetupResult::FromRenderer(Dx12RendererSetupContext::CreateGraphicsCommandList, Dx12ResultCode::CreateCommandListFailed, hr);

		hr = m_commandList->Close();

		if (FAILED(hr))
			return Dx12RendererSetupResult::FromRenderer(Dx12RendererSetupContext::CreateGraphicsCommandList, Dx12ResultCode::GraphicsCommandListCloseFailed, hr);

		return {};
	}
}
