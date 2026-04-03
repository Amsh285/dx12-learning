#pragma once

#include "Dx12FenceTypes.h"
#include "Logger.h"

namespace directx12
{
	

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
	};
}
