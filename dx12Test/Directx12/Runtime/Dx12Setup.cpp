
#include "pch.h"

#include "Dx12Setup.h"
#include "Dx12Runtime.h"

using namespace Microsoft::WRL;

namespace directx12
{
	namespace runtime
	{
		Dx12SetupResult EnableDebugLayer();

		Dx12SetupResult GetAdapter(bool useWarp);

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

			if (result.status != Dx12ResultCode::Success)
			{
				logger.Error("Failed to activate debug layer. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
				return result;
			}

			result = GetAdapter(false);

			if (result.status != Dx12ResultCode::Success)
			{
				logger.Error("Failed to retrieve Adapter. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
				return result;
			}

			HRESULT hr = D3D12CreateDevice(g_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(g_device.GetAddressOf()));

			if (FAILED(hr))
			{
				logger.Error("Failed to create Device. Setup state: {0}. Error code: {1}", static_cast<int>(Dx12ResultCode::D3D12CreateDeviceFailed), hr);
				return { Dx12SetupContext::CreateDevice, Dx12ResultCode::D3D12CreateDeviceFailed, hr };
			}

			result = ConfigureInfoQueue();

			if (result.status != Dx12ResultCode::Success)
				logger.Warn("Failed to configure Infoqueue. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);

			logger.Info("Dx12 runtime setup completed successful.");
			return result;
		}

		Dx12SetupResult EnableDebugLayer()
		{
#if defined(_DEBUG)
			ComPtr<ID3D12Debug> debugInterface;
			HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));

			if (FAILED(result))
				return { Dx12SetupContext::EnableDebugLayer, Dx12ResultCode::QueryDebugInterfaceFailed, result };

			debugInterface->EnableDebugLayer();
#endif
			return {};
		}

		Dx12SetupResult GetAdapter(bool useWarp)
		{
			using namespace Microsoft::WRL;

			assert(g_adapter == nullptr);

			ComPtr<IDXGIFactory4> dxgiFactory = nullptr;
			uint32_t createFactoryFlags = 0;

#if defined(_DEBUG)
			createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // DEBUG

			HRESULT result = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory));

			if (FAILED(result))
				return { Dx12SetupContext::GetAdapter, Dx12ResultCode::CreateDXGIFactoryFailed, result };

			if (useWarp)
			{
				// Query the WARP adapter (software / virtual adapter)
				ComPtr<IDXGIAdapter1> adapter1 = nullptr;
				result = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter1));

				if (FAILED(result))
					return { Dx12SetupContext::GetAdapter, Dx12ResultCode::EnumWarpAdapterFailed, result };

				ComPtr<IDXGIAdapter4> adapterT;
				result = adapter1.As(&adapterT);

				if (FAILED(result))
					return { Dx12SetupContext::GetAdapter, Dx12ResultCode::ComInterfaceCastFailed, result };

				g_adapter = adapterT.Detach();
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
					return { Dx12SetupContext::GetAdapter, Dx12ResultCode::NoHardwareAdapterFound, E_FAIL };

				ComPtr<IDXGIAdapter4> bestAdapterT;
				result = bestAdapter1.As(&bestAdapterT);

				if (FAILED(result))
					return { Dx12SetupContext::GetAdapter, Dx12ResultCode::ComInterfaceCastFailed, result };

				g_adapter = bestAdapterT.Detach();
			}

			return {};
		}

		Dx12SetupResult ConfigureInfoQueue()
		{
#if defined(_DEBUG)
			assert(g_device != nullptr);

			ComPtr<ID3D12InfoQueue> infoQueue;
			HRESULT hr = g_device.As(&infoQueue);

			if (FAILED(hr))
				return { Dx12SetupContext::ConfigureInfoQueue, Dx12ResultCode::ComInterfaceCastFailed, hr };

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

			// try to fix warnings on debug build
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			hr = infoQueue->PushStorageFilter(&NewFilter);

			if (FAILED(hr))
				return { Dx12SetupContext::ConfigureInfoQueue, Dx12ResultCode::PushStorageFilterFailed, hr };

#endif
			return {};
		}
	}
}
