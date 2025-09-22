#pragma once

#include "Logger.h"

namespace dx12Runtime
{
	enum class Dx12SetupContext
	{
		Undefined,
		EnableDebugLayer,
		GetAdapter,
		CreateDevice,
		ConfigureInfoQueue
	};

	enum class Dx12SetupStatus
	{
		Success,

		QueryDebugInterfaceFailed,

		DxGiCastFailed,

		CreateDXGIFactory2Failed,

		EnumWarpAdapterFailed,
		NoHardwareAdapterFound,
		QueryIDXGIAdapter4Failed,

		D3D12CreateDeviceFailed,

		PushStorageFilterFailed
	};

	struct Dx12SetupResult
	{
		Dx12SetupContext context = Dx12SetupContext::Undefined;
		Dx12SetupStatus status = Dx12SetupStatus::Success;
		HRESULT code = S_OK;
	};

	Dx12SetupResult Setup();
}
