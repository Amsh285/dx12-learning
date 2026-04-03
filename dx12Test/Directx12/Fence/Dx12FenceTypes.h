#pragma once

#include "Dx12ResultCode.h"

namespace directx12
{
	enum class Dx12FenceSetupContext
	{
		Undefined,
		CreateFence,
		CreateEventHandle
	};

	struct Dx12FenceSetupResult
	{
		Dx12FenceSetupContext context = Dx12FenceSetupContext::Undefined;
		Dx12ResultCode status = Dx12ResultCode::Success;
		HRESULT hr = S_OK;
	};
}
