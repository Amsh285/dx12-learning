#include "pch.h"

#include "Dx12Renderer.h"
#include "Logger.h"

#include "Runtime/Dx12Runtime.h"

using namespace Microsoft::WRL;

namespace directx12
{
	Dx12Renderer::Dx12Renderer(const windows::WindowData& windowData)
		: m_windowData(windowData)
	{
	}

	Dx12RendererSetupResult Dx12Renderer::Setup()
	{
		using namespace directx12::runtime;

		Logger logger("Dx12Renderer");
		logger.Info("Initializing Dx12Renderer.");

		Dx12RendererSetupResult result = CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		if (result.status != Dx12ResultCode::Success)
		{
			logger.Error("Failed to create CommandQueue. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}



		
		logger.Info("Dx12Renderer initialization complete.");
		return {};
	}

	Dx12RendererSetupResult Dx12Renderer::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		HRESULT hr = runtime::g_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));

		if (FAILED(hr))
			return { Dx12RendererSetupContext::CreateCommandQueue, Dx12ResultCode::CreateCommandQueueFailed, hr };

		return {};
	}
}
