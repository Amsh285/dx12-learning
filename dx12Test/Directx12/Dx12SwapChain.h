#pragma once

#include "Dx12ResultCode.h"

#include "Logger.h"
#include "Windows/WindowData.h"

namespace directx12
{
	enum class Dx12SwapChainSetupContext
	{
		Undefined,
		CreateSwapChain,
		CreateRTVDescriptorHeap,
		UpdateRenderTargetViews
	};

	struct Dx12SetupSwapChainResult
	{
		Dx12SwapChainSetupContext context = Dx12SwapChainSetupContext::Undefined;
		Dx12ResultCode status = Dx12ResultCode::Success;
		HRESULT code = S_OK;
	};

	class Dx12SwapChain
	{
	public:
		UINT GetBufferCount() const { return m_bufferCount; }

		Dx12SwapChain();
		Dx12SwapChain(UINT bufferCount, bool vSync);
		~Dx12SwapChain();

		void Release();

		Dx12SetupSwapChainResult Setup(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue, const windows::WindowData& windowData);

	private:
		Dx12SetupSwapChainResult CreateSwapChain();
		Dx12SetupSwapChainResult CreateRTVDescriptorHeap();
		Dx12SetupSwapChainResult UpdateRenderTargetViews();

		bool m_vSync;
		UINT m_bufferCount;
		UINT m_currentBackBufferIndex = 0;

		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_backBuffers;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_rtvHandles;
		
		std::vector<uint64_t> m_frameFenceValues;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;

		windows::WindowData m_windowData;
		Logger m_logger;

		bool m_released;
	};
}
