#include "pch.h"
#include "Dx12ResultHandler.h"

void Dx12ResultHandler::Report(const directx12::Dx12RendererSetupResult& result)
{
	using namespace directx12;

	static Logger logger("Dx12ResultHandler");

	if (result.status == Dx12ResultCode::Success)
		return;

	std::string context;

	switch (result.context)
	{
	case Dx12RendererSetupContext::CreateCommandQueue:
		context = "Failed to create CommandQueue.";
		break;

		
	}
}
