#pragma once

#include "Dx12ResultCode.h"

namespace directx12
{
#define DX12_FENCE_SETUP_CONTEXT(X) \
	X(Undefined) \
	X(CreateFence) \
	X(CreateEventHandle)

	enum class Dx12FenceSetupContext
	{
#define X(name) name,
		DX12_FENCE_SETUP_CONTEXT(X)
#undef X
	};

	inline constexpr const char* to_string(Dx12FenceSetupContext ctx)
	{
		switch (ctx)
		{
#define X(name) case Dx12FenceSetupContext::name: return #name;
			DX12_FENCE_SETUP_CONTEXT(X)
#undef X
		default:
			return "Unknown Dx12FenceSetupContext";
		}
	}

	struct Dx12FenceSetupResult
	{
		Dx12FenceSetupContext context = Dx12FenceSetupContext::Undefined;
		Dx12ResultCode status = Dx12ResultCode::Success;
		HRESULT hr = S_OK;
	};
}
