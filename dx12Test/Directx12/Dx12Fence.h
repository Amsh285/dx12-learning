#pragma once

#include <Dx12ResultCode.h>
#include "Logger.h"

namespace directx12
{
	enum class Dx12FenceSetupContext
	{
		Undefined,
		CreateFence,
		CreateEventHandle
	};

	struct Dx12FenceSetupResult
	{
		Dx12FenceSetupContext context = Dx12FenceSetupContext::Undefined;
		Dx12ResultCode status = Dx12ResultCode::Success;
		HRESULT code = S_OK;
	};

	class Dx12Fence
	{
	public:
		uint64_t GetFenceValue() const { return m_fenceValue; }

		Dx12Fence();
		~Dx12Fence();
		void Release();

		Dx12FenceSetupResult Setup(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& queue);

		uint64_t Signal();
		void WaitCpu(uint64_t fenceValue) const;

		void Flush();
	private:
		Dx12FenceSetupResult CreateFence();
		bool CreateEventHandle();

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;

		HANDLE m_fenceEvent;
		std::atomic<uint64_t> m_fenceValue;

		Logger m_logger;

		bool m_released;
	};
}
