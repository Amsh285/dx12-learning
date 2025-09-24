#include "pch.h"

#include "Dx12Renderer.h"
#include "Logger.h"

#include "Runtime/Dx12Runtime.h"

namespace directx12
{
	std::shared_ptr<Dx12Renderer> Dx12Renderer::Get()
	{
		struct enable_make_shared : public Dx12Renderer {};
		static std::shared_ptr<Dx12Renderer> s_renderer = std::make_shared<enable_make_shared>();

		return s_renderer;
	}

	bool Dx12Renderer::Setup()
	{
		using namespace directx12::runtime;

		Logger logger("Dx12Renderer");
		logger.Info("Initializing Dx12Renderer.");


		logger.Info("Dx12Renderer initialization complete.");
		return true;
	}

	HRESULT Dx12Renderer::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		return runtime::g_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
	}

	bool Dx12Renderer::CheckTearingSupport()
	{
		using namespace Microsoft::WRL;

		ComPtr<IDXGIFactory4> factory4;

		/*if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4)))
		{
		}*/

		return true;
	}
}
