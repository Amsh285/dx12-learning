#pragma once

#include "Logger.h"

namespace dx12Runtime
{
	enum class Dx12SetupContext
	{
		Undefined,
		EnableDebugLayer,
		GetAdapter,
		CreateDevice
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

		D3D12CreateDeviceFailed
	};

	struct Dx12SetupResult
	{
		Dx12SetupContext context = Dx12SetupContext::Undefined;
		Dx12SetupStatus status = Dx12SetupStatus::Success;
		HRESULT code = S_OK;
	};


	extern Microsoft::WRL::ComPtr<IDXGIAdapter4> g_adapter;
	extern Microsoft::WRL::ComPtr<ID3D12Device10> g_device;

	Dx12SetupResult Setup();
}
