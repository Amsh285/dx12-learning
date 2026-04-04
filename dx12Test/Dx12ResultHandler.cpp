#include "pch.h"
#include "Dx12ResultHandler.h"

void Dx12ResultHandler::Report(const directx12::Dx12RendererSetupResult& result)
{
	using namespace directx12;

	static Logger logger("Dx12ResultHandler");

	if (result.status == Dx12ResultCode::Success)
		return;

	const char* context = GetSystemContext(result);
	const char* system =
		result.subsystem == Dx12RendererSubsystem::None
		? "Renderer"
		: to_string(result.subsystem);

	logger.Error(
		"system: {0} context: {1} state: {2} hr: 0x{3:X}",
		system, context, result.status, result.hr
	);
}

const char* Dx12ResultHandler::GetSystemContext(const directx12::Dx12RendererSetupResult& result)
{
	using namespace directx12;

	switch (result.subsystem)
	{
	case Dx12RendererSubsystem::SwapChain:
		return to_string(result.subContext.swapChain);
	case Dx12RendererSubsystem::Fence:
		return to_string(result.subContext.fence);
	case Dx12RendererSubsystem::None:
		return to_string(result.context);
	default:
		return "invalid subsystem";
	}
}
