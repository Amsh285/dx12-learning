#pragma once

namespace directx12
{
	namespace runtime
	{
		extern Microsoft::WRL::ComPtr<IDXGIAdapter4> g_adapter;
		extern Microsoft::WRL::ComPtr<ID3D12Device10> g_device;

		extern bool g_tearingSupported;

		void ReportLiveObjects();
	}
}
