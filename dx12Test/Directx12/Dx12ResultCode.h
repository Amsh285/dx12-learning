#pragma once

namespace directx12
{
	enum class Dx12ResultCode
	{
		// General
		Success,
		ComInterfaceCastFailed,
		UnknownError,

		// Runtime / Setup errors
		QueryDebugInterfaceFailed,
		CreateDXGIFactoryFailed,
		EnumWarpAdapterFailed,
		NoHardwareAdapterFound,
		D3D12CreateDeviceFailed,
		PushStorageFilterFailed

		// Renderer / Command queue errors
	};
}
