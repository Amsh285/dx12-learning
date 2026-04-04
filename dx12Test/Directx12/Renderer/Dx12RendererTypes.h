#pragma once

#include "Fence/Dx12FenceTypes.h"
#include "SwapChain/Dx12SwapChainTypes.h"
#include "Dx12ResultCode.h"

namespace directx12
{
#define DX12_RENDERER_SETUP_CONTEXTS(X) \
    X(Undefined) \
    X(CreateCommandQueue) \
    X(CreateCommandAllocators) \
    X(CreateGraphicsCommandList)

	enum class Dx12RendererSetupContext
	{
#define X(name) name,
		DX12_RENDERER_SETUP_CONTEXTS(X)
#undef X
	};

	inline constexpr const char* to_string(Dx12RendererSetupContext ctx)
	{
		switch (ctx)
		{
#define X(name) case Dx12RendererSetupContext::name: return #name;
			DX12_RENDERER_SETUP_CONTEXTS(X)
#undef X
		default:
			return "unknown Dx12RendererSetupContext";
		}
	}

#define DX12_RENDERER_SUBSYSTEM(X) \
	X(None) \
	X(SwapChain) \
	X(Fence)

	enum class Dx12RendererSubsystem
	{
#define X(name) name,
		DX12_RENDERER_SUBSYSTEM(X)
#undef X
	};

	inline constexpr const char* to_string(Dx12RendererSubsystem subSystem)
	{
		switch (subSystem)
		{
#define X(name) case Dx12RendererSubsystem::name: return #name;
			DX12_RENDERER_SUBSYSTEM(X)
#undef X
		default:
			return "unknown Dx12RendererSubsystem";
		}
	}

	struct Dx12RendererSetupResult
	{
		Dx12RendererSetupContext context = Dx12RendererSetupContext::Undefined;
		Dx12RendererSubsystem subsystem = Dx12RendererSubsystem::None;
		Dx12ResultCode status = Dx12ResultCode::Success;
		HRESULT hr = S_OK;

		union
		{
			
			Dx12SwapChainSetupContext swapChain;
			Dx12FenceSetupContext fence;
		} subContext;

		static Dx12RendererSetupResult FromSwapChain(
			Dx12SwapChainSetupContext scContext,
			Dx12ResultCode status,
			HRESULT hr)
		{
			Dx12RendererSetupResult ctx{};
			ctx.subsystem = Dx12RendererSubsystem::SwapChain;
			ctx.status = status;
			ctx.hr = hr;
			ctx.subContext.swapChain = scContext;

			return ctx;
		}

		static Dx12RendererSetupResult FromFence(
			Dx12FenceSetupContext fContext,
			Dx12ResultCode status,
			HRESULT hr)
		{
			Dx12RendererSetupResult ctx{};
			ctx.subsystem = Dx12RendererSubsystem::Fence;
			ctx.status = status;
			ctx.hr = hr;
			ctx.subContext.fence = fContext;

			return ctx;
		}

		static Dx12RendererSetupResult FromRenderer(
			Dx12RendererSetupContext context,
			Dx12ResultCode status,
			HRESULT hr)
		{
			Dx12RendererSetupResult ctx{};
			ctx.subsystem = Dx12RendererSubsystem::None;
			ctx.status = status;
			ctx.hr = hr;
			ctx.context = context;

			return ctx;
		}
	};
}
