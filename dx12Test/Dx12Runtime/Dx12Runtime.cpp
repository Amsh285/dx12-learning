#include "pch.h"
#include "Dx12Runtime.h"

using namespace Microsoft::WRL;

namespace dx12Runtime
{
	ComPtr<IDXGIAdapter4> g_adapter;
	ComPtr<ID3D12Device10> g_device;
}
