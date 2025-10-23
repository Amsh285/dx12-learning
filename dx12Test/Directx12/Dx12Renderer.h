#pragma once

#include "Dx12ResultCode.h"
#include "Dx12Fence.h"
#include "Dx12SwapChain.h"

#include "Logger.h"
#include "Windows/WindowData.h"

namespace directx12
{
	enum class Dx12RendererSetupContext
	{
		Undefined,
		CreateCommandQueue,
		SetupInternalSwapChainStructure,
		CreateCommandAllocators,
		CreateGraphicsCommandList,
		SetupInternalFenceStructure
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
		Dx12Renderer();
		~Dx12Renderer();
		void Release();

		Dx12RendererSetupResult Setup(const windows::WindowData& windowData);

		void Render();
		void Flush();
	private:
		Dx12RendererSetupResult CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type);
		Dx12RendererSetupResult CreateCommandAllocators();
		Dx12RendererSetupResult CreateGraphicsCommandList();

		UINT m_frameCount = 3;

		std::vector<uint64_t> m_frameFenceValues;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> m_commandAllocators;

		Dx12SwapChain m_swapChain;
		Dx12Fence m_fence;    

		Logger m_logger;
	};
}
