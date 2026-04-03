#pragma once

#include "Dx12ResultCode.h"

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
		HRESULT hr = S_OK;
	};
}
