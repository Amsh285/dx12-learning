#include "pch.h"

#include "Dx12Fence.h"

#include "Runtime/Dx12Runtime.h"

namespace directx12
{
	Dx12Fence::Dx12Fence()
		: m_fenceEvent(nullptr), m_fenceValue(0), m_released(false),
		m_logger([]() {
		static std::atomic<uint32_t> s_fenceInstance = 0;
		uint32_t fenceId = s_fenceInstance.fetch_add(1);

		return std::string("Dx12Fence#") + std::to_string(fenceId);
			}())
	{
	}

	Dx12Fence::~Dx12Fence()
	{
		if (!m_released)
			Release();
	}

	void Dx12Fence::Release()
	{
		m_released = true;

		if (m_fenceEvent)
		{
			CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
		}

		m_fence.Reset();
		m_commandQueue.Reset();
		m_fenceValue = 0;
	}

	Dx12FenceSetupResult Dx12Fence::Setup(const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& queue)
	{
		m_commandQueue = queue;

		Dx12FenceSetupResult result = CreateFence();

		if (result.status != Dx12ResultCode::Success)
		{
			m_logger.Error("Failed to create Fence. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return result;
		}

		if (!CreateEventHandle())
		{
			m_logger.Error("Failed to create EventHandle. Setup state: {0}. Error code: {1}", static_cast<int>(result.status), result.code);
			return { Dx12FenceSetupContext::CreateEventHandle, Dx12ResultCode::UnknownError, E_FAIL };
		}

		return {};
	}

	uint64_t Dx12Fence::Signal()
	{
		uint64_t fenceValueForSignal = ++m_fenceValue;
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceValueForSignal));

		return fenceValueForSignal;
	}

	void Dx12Fence::WaitCpu(uint64_t fenceValue) const
	{
		if (m_fence->GetCompletedValue() >= fenceValue)
			return;

		ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));

		size_t retryCount = 0;
		const size_t maxRetries = 3;
		DWORD result = 0;

		do
		{
			++retryCount;
			result = WaitForSingleObject(m_fenceEvent, 2000);

			if (result == WAIT_FAILED)
			{
				m_logger.Error("Failed to wait for Fence event");
				throw std::runtime_error("Failed to wait for Fence event.");
			}

		} while (result != WAIT_OBJECT_0 && retryCount < maxRetries);

		if (result == WAIT_TIMEOUT)
		{
			m_logger.Error("Failed to wait for Fence event. Timeout period elapsed.");
			throw std::runtime_error("Failed to wait for Fence event. Timeout period elapsed.");
		}
	}

	void Dx12Fence::Flush()
	{
		uint64_t fenceValueForSignal = Signal();
		WaitCpu(fenceValueForSignal);
	}

	Dx12FenceSetupResult Dx12Fence::CreateFence()
	{
		HRESULT hr = runtime::g_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

		if (FAILED(hr))
			return { Dx12FenceSetupContext::CreateFence, Dx12ResultCode::CreateFenceFailed, hr };

		return {};
	}

	bool Dx12Fence::CreateEventHandle()
	{
		m_fenceEvent = CreateEventExW(NULL, FALSE, FALSE, NULL);
		return m_fenceEvent != nullptr;
	}
}
