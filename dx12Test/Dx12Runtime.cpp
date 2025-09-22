
#include "pch.h"
#include "Dx12Runtime.h"

using namespace Microsoft::WRL;

namespace dx12Runtime
{
	ComPtr<IDXGIAdapter4> g_adapter;
	ComPtr<ID3D12Device10> g_device;

	Dx12SetupResult EnableDebugLayer();

	Dx12SetupResult GetAdapter(bool useWarp, IDXGIAdapter4** adapter);

	Dx12SetupResult ConfigureInfoQueue();

	Dx12SetupResult Setup()
	{
		Logger logger("Dx12 runtime setup");

		if (g_device)
		{
			logger.Warn("Dx12 device already initialized. Cancelling runtime setup.");
			return {};
		}

		logger.Info("Starting Dx12 runtime setup.");

		Dx12SetupResult result = EnableDebugLayer();

		if (result.status != Dx12SetupStatus::Success)
		{
			logger.Error("Failed to activate debug layer. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		result = GetAdapter(false, g_adapter.GetAddressOf());

		if (result.status != Dx12SetupStatus::Success)
		{
			logger.Error("Failed to retrieve Adapter. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		HRESULT hr = D3D12CreateDevice(g_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(g_device.GetAddressOf()));

		if (FAILED(hr))
		{
			logger.Error("Failed to create Device. Setup state: {0}. Error code: {1}", static_cast<int>(Dx12SetupStatus::D3D12CreateDeviceFailed), hr);
			return { Dx12SetupContext::CreateDevice, Dx12SetupStatus::D3D12CreateDeviceFailed, hr };
		}

		logger.Info("Dx12 runtime setup completed successful.");
		return result;
	}

	Dx12SetupResult EnableDebugLayer()
	{
#if defined(_DEBUG)
		ComPtr<ID3D12Debug> debugInterface;
		HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));

		if (FAILED(result))
			return { Dx12SetupContext::EnableDebugLayer, Dx12SetupStatus::QueryDebugInterfaceFailed, result };

		debugInterface->EnableDebugLayer();
#endif
		return {};
	}

	Dx12SetupResult GetAdapter(bool useWarp, IDXGIAdapter4** adapter)
	{
		using namespace Microsoft::WRL;

		assert(*adapter == nullptr);

		ComPtr<IDXGIFactory4> dxgiFactory = nullptr;
		uint32_t createFactoryFlags = 0;

#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // DEBUG

		HRESULT result = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory));

		if (FAILED(result))
			return { Dx12SetupContext::GetAdapter, Dx12SetupStatus::CreateDXGIFactory2Failed, result };

		if (useWarp)
		{
			// Query the WARP adapter (software / virtual adapter)
			ComPtr<IDXGIAdapter1> adapter1 = nullptr;
			result = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter1));

			if (FAILED(result))
				return { Dx12SetupContext::GetAdapter, Dx12SetupStatus::EnumWarpAdapterFailed, result };

			ComPtr<IDXGIAdapter4> adapterT;
			result = adapter1.As(&adapterT);

			if (FAILED(result))
				return { Dx12SetupContext::GetAdapter, Dx12SetupStatus::DxGiCastFailed, result };

			*adapter = adapterT.Detach();
		}
		else
		{
			// Enumerate hardware adapters and pick the one with the largest dedicated video memory
			ComPtr<IDXGIAdapter1> adapter1 = nullptr;
			ComPtr<IDXGIAdapter1> bestAdapter1 = nullptr;
			SIZE_T maxMemory = 0;

			for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &adapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter1->GetDesc1(&desc);

				if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
					SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
					desc.DedicatedVideoMemory > maxMemory)
				{
					maxMemory = desc.DedicatedVideoMemory;
					bestAdapter1 = adapter1;
				}

				adapter1.Reset();
			}

			if (!bestAdapter1)
				return { Dx12SetupContext::GetAdapter, Dx12SetupStatus::NoHardwareAdapterFound, E_FAIL };

			ComPtr<IDXGIAdapter4> bestAdapterT;
			result = bestAdapter1.As(&bestAdapterT);

			if (FAILED(result))
				return { Dx12SetupContext::GetAdapter, Dx12SetupStatus::DxGiCastFailed, result };

			*adapter = bestAdapterT.Detach();
		}

		return {};
	}
}
