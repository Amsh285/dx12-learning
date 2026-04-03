#pragma once

#include "Fence/Dx12FenceTypes.h"
#include "SwapChain/Dx12SwapChainTypes.h"
#include "Dx12ResultCode.h"

namespace directx12
{
    enum class Dx12RendererSetupContext
    {
        Undefined,
        CreateCommandQueue,
        SetupInternalSwapChainStructure,
        CreateCommandAllocators,
        CreateGraphicsCommandList,
        SetupInternalFenceStructure
    };

    struct Dx12RendererSetupResult
    {
        Dx12RendererSetupContext context = Dx12RendererSetupContext::Undefined;
        Dx12SwapChainSetupContext swapChainContext = Dx12SwapChainSetupContext::Undefined;
        Dx12FenceSetupContext fenceContext = Dx12FenceSetupContext::Undefined;
        Dx12ResultCode status = Dx12ResultCode::Success;
        HRESULT hr = S_OK;

        static Dx12RendererSetupResult FromSwapChain(
            Dx12SwapChainSetupContext scContext,
            Dx12ResultCode status,
            HRESULT hr)
        {
            return {
                Dx12RendererSetupContext::SetupInternalSwapChainStructure,
                scContext,
                Dx12FenceSetupContext::Undefined,
                status,
                hr
            };
        }

        static Dx12RendererSetupResult FromFence(
            Dx12FenceSetupContext fContext,
            Dx12ResultCode status,
            HRESULT hr)
        {
            return {
                Dx12RendererSetupContext::Undefined,
                Dx12SwapChainSetupContext::Undefined,
                fContext,
                status,
                hr
            };
        }

        static Dx12RendererSetupResult FromRenderer(
            Dx12RendererSetupContext context,
            Dx12ResultCode status,
            HRESULT hr)
        {
            return {
                context,
                Dx12SwapChainSetupContext::Undefined,
                Dx12FenceSetupContext::Undefined,
                status,
                hr
            };
        }
    };
}
