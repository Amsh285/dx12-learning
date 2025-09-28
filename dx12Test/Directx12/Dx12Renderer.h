#pragma once

#include "Dx12ResultCode.h"
#include "Windows/WindowData.h"

namespace directx12
{
	enum class Dx12RendererSetupContext
	{
		Undefined,
		CreateCommandQueue,
		CreateSwapChain,
		CreateRTVDescriptorHeap,
		UpdateRenderTargetViews
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
	private:
		Dx12RendererSetupResult CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type);
		Dx12RendererSetupResult CreateSwapChain();
		Dx12RendererSetupResult CreateRTVDescriptorHeap();
		Dx12RendererSetupResult UpdateRenderTargetViews();

		UINT m_frameCount = 3;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_backBuffers;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_rtvHandles;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;


		windows::WindowData m_windowData;
	};
}
