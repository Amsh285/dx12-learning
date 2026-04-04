#pragma once

#include "Dx12ResultCode.h"

namespace directx12
{
#define DX12_SWAPCHAIN_SETUP_CONTEXT(X) \
	X(Undefined) \
	X(CreateSwapChain) \
	X(CreateRTVDescriptorHeap) \
	X(UpdateRenderTargetViews)

	enum class Dx12SwapChainSetupContext
	{
#define X(name) name,
		DX12_SWAPCHAIN_SETUP_CONTEXT(X)
#undef X
	};

	inline constexpr const char* to_string(Dx12SwapChainSetupContext ctx)
	{
		switch (ctx)
		{
#define X(name) case Dx12SwapChainSetupContext::name: return #name;
			DX12_SWAPCHAIN_SETUP_CONTEXT(X)
#undef X
		default:
			return "Unknown Dx12SwapChainSetupContext";
		}
	}

	struct Dx12SetupSwapChainResult
	{
		Dx12SwapChainSetupContext context = Dx12SwapChainSetupContext::Undefined;
		Dx12ResultCode status = Dx12ResultCode::Success;
		HRESULT hr = S_OK;
	};
}
