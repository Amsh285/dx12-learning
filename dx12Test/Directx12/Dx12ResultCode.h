#pragma once

namespace directx12
{
	enum class Dx12ResultCode
	{
		Success,
		UnknownError,

		ComInterfaceCastFailed,

		CreateDXGIFactoryFailed,
		CreateDeviceFailed,
		CreateCommandQueueFailed,
		CreateSwapChainFailed,
		CreateDescriptorHeapFailed,
		CreateBufferFailed,
		CreateCommandAllocatorFailed,
		CreateCommandListFailed,
		CreateFenceFailed,
		
		QueryDebugInterfaceFailed,
		EnumWarpAdapterFailed,
		NoHardwareAdapterFound,
		MakeWindowAssociationFailed,
		PushStorageFilterFailed,
		DXGIFactoryCheckFeatureSupportFailed,
		GraphicsCommandListCloseFailed
	};
}
