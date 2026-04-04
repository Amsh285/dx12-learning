#pragma once

namespace directx12
{
#define DX12_RESULT_CODE(X) \
		X(Success) \
		X(UnknownError) \
		\
		X(ComInterfaceCastFailed) \
		\
		X(CreateDXGIFactoryFailed) \
		X(CreateDeviceFailed) \
		X(CreateCommandQueueFailed) \
		X(CreateSwapChainFailed) \
		X(CreateDescriptorHeapFailed) \
		X(CreateBufferFailed) \
		X(CreateCommandAllocatorFailed) \
		X(CreateCommandListFailed) \
		X(CreateFenceFailed) \
		\
		X(QueryDebugInterfaceFailed) \
		X(EnumWarpAdapterFailed) \
		X(NoHardwareAdapterFound) \
		X(MakeWindowAssociationFailed) \
		X(PushStorageFilterFailed) \
		X(DXGIFactoryCheckFeatureSupportFailed) \
		X(GraphicsCommandListCloseFailed)

	enum class Dx12ResultCode
	{
#define X(name) name,
		DX12_RESULT_CODE(X)
#undef X
	};

	inline constexpr const char* to_string(Dx12ResultCode code)
	{
		switch (code)
		{
#define X(name) case Dx12ResultCode::name: return #name;
			DX12_RESULT_CODE(X)
#undef X
		default:
			return "Unknown Dx12ResultCode";
		}
	}
}
