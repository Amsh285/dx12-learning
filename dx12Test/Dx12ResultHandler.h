#pragma once

#include "Logger.h"
#include "Directx12/Fence/Dx12FenceTypes.h"
#include "Directx12/Swapchain/Dx12SwapChainTypes.h"
#include "Directx12/Renderer/Dx12RendererTypes.h"

class Dx12ResultHandler
{
public:
	static void Report(const directx12::Dx12RendererSetupResult& result);
private:
	static const char* GetSystemContext(const directx12::Dx12RendererSetupResult& result);
};
