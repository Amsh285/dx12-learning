#pragma once

namespace directx12
{
	enum class Dx12ResultCode
	{
		// General
		Success,
		ComInterfaceCastFailed,
		CreateDXGIFactoryFailed,
		DXGIFactoryCheckFeatureSupportFaile,
		UnknownError,

		// Runtime / Setup errors
		QueryDebugInterfaceFailed,
		EnumWarpAdapterFailed,
		NoHardwareAdapterFound,
		D3D12CreateDeviceFailed,
		PushStorageFilterFailed,

		// Renderer / Command queue errors
		CreateCommandQueueFailed
	};
}
