#pragma once

namespace directx12
{
	class Dx12Renderer
	{
	public:
		static std::shared_ptr<Dx12Renderer> Get();

		bool Setup();
	private:
		Dx12Renderer() = default;

		HRESULT CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type);
		bool CheckTearingSupport();

		
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

		bool m_tearingSupported = false;
	};
}
