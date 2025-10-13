#pragma once

#include "Dx12ResultCode.h"
#include "Dx12Fence.h"

#include "Logger.h"
#include "Windows/WindowData.h"

namespace directx12
{
	enum class Dx12RendererSetupContext
	{
		Undefined,
		CreateCommandQueue,
		CreateSwapChain,
		CreateRTVDescriptorHeap,
		UpdateRenderTargetViews,
		CreateCommandAllocators,
		CreateGraphicsCommandList,
		CreateFence,
		CreateEventHandle
	};

	struct Dx12RendererSetupResult
	{
		Dx12RendererSetupContext context = Dx12RendererSetupContext::Undefined;
		Dx12ResultCode status = Dx12ResultCode::Success;
		HRESULT code = S_OK;
	};

	class Dx12Renderer
	{
	public:
		Dx12Renderer(const windows::WindowData& windowData);

		Dx12RendererSetupResult Setup();

		void Render();
		void Flush();
	private:
		Dx12RendererSetupResult CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type);
		Dx12RendererSetupResult CreateSwapChain();
		Dx12RendererSetupResult CreateRTVDescriptorHeap();
		Dx12RendererSetupResult UpdateRenderTargetViews();
		Dx12RendererSetupResult CreateCommandAllocators();
		Dx12RendererSetupResult CreateGraphicsCommandList();

		bool m_vSync = true;
		UINT m_frameCount = 3;
		UINT m_currentBackBufferIndex = 0;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_backBuffers;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_rtvHandles;
		std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_commandAllocators;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		Dx12Fence m_fence;
		std::vector<uint64_t> m_frameFenceValues;

		windows::WindowData m_windowData;
		Logger m_logger;
	};
}
