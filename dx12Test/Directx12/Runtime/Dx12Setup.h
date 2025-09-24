#pragma once

#include "Dx12ResultCode.h"
#include "Logger.h"

namespace directx12
{
	namespace runtime
	{
		enum class Dx12SetupContext
		{
			Undefined,
			EnableDebugLayer,
			GetAdapter,
			CreateDevice,
			ConfigureInfoQueue
		};

		struct Dx12SetupResult
		{
			Dx12SetupContext context = Dx12SetupContext::Undefined;
			Dx12ResultCode status = Dx12ResultCode::Success;
			HRESULT code = S_OK;
		};

		Dx12SetupResult Setup();
	}
}
