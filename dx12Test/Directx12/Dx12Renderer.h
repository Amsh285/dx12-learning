#pragma once

#include "Dx12ResultCode.h"
#include "Windows/WindowData.h"

namespace directx12
{
	enum class Dx12RendererSetupContext
	{
		Undefined,
		CreateCommandQueue
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
		
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

		windows::WindowData m_windowData;
	};
}
