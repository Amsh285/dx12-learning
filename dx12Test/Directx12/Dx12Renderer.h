#pragma once

#include "Dx12ResultCode.h"

namespace directx12
{
	enum class Dx12RendererSetupContext
	{
		Undefined,
		CreateCommandQueue,
		CheckTearingSupport
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
		static std::shared_ptr<Dx12Renderer> Get();

		Dx12RendererSetupResult Setup();
	private:
		Dx12Renderer() = default;

		Dx12RendererSetupResult CheckTearingSupport();
		Dx12RendererSetupResult CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type);
		
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

		bool m_tearingSupported = false;
	};
}
