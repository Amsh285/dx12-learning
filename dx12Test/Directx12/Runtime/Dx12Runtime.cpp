#include "pch.h"
#include "Dx12Runtime.h"

using namespace Microsoft::WRL;

namespace directx12
{
	namespace runtime
	{
		ComPtr<IDXGIAdapter4> g_adapter;
		ComPtr<ID3D12Device10> g_device;

		bool g_tearingSupported = false;

		void ReportLiveObjects()
		{
#if defined(_DEBUG)
			ComPtr<IDXGIDebug1> dxgiDebug;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
			{
				dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
			}
#endif
		}
	}
}
