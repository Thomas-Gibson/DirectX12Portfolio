#include "Dx12DeviceContext.hpp"

Dx12Framework::Dx12DeviceContext::Dx12DeviceContext() :
	pDxgiFactory(InitFactory()),
	pDxgiHardwareAdapter(InitHardwareAdapter()),
	pDxDevice(InitDevice()),
	dxDirectQueue(pDxDevice.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
	dxCopyQueue(pDxDevice.Get(), D3D12_COMMAND_LIST_TYPE_COPY)
{
}

Microsoft::WRL::ComPtr<IDXGIFactory7> Dx12Framework::Dx12DeviceContext::InitFactory()
{
	Microsoft::WRL::ComPtr<IDXGIFactory7> pOutDxgiFactory = nullptr;
	constexpr UINT dxgiFactoryFlags = EnableDebugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pOutDxgiFactory)));

	return pOutDxgiFactory.Get();
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> Dx12Framework::Dx12DeviceContext::InitHardwareAdapter()
{
	Microsoft::WRL::ComPtr<IDXGIAdapter4> pOutDxgiAdapter = nullptr;

	auto GetAdapter = [&](UINT index) -> HRESULT {
		return Factory()->EnumAdapterByGpuPreference(
			index,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(pOutDxgiAdapter.GetAddressOf()));
	};

	for (UINT adapterIndex = 0;
		 SUCCEEDED(GetAdapter(adapterIndex));
		 adapterIndex++) {

		// Get the adapter description
		DXGI_ADAPTER_DESC3 adapterDesc = {};
		if (FAILED(pOutDxgiAdapter->GetDesc3(&adapterDesc))) {
			continue;
		}

		// Skip software adapters
		const bool isSoftwareAdapter = (adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) != 0;
		if (isSoftwareAdapter) {
			pOutDxgiAdapter.Reset();
			continue;
		}

		// Check if the adapter supports DirectX12
		Microsoft::WRL::ComPtr<ID3D12Device> pTestDevice;
		const HRESULT deviceResult =
			D3D12CreateDevice(pOutDxgiAdapter.Get(),
							  D3D_FEATURE_LEVEL_12_2,
							  IID_PPV_ARGS(pTestDevice.GetAddressOf()));


		if (SUCCEEDED(deviceResult)) break;

		pOutDxgiAdapter.Reset();
	}

	return pOutDxgiAdapter.Get();
}

Microsoft::WRL::ComPtr<ID3D12Device14> Dx12Framework::Dx12DeviceContext::InitDevice()
{
	Microsoft::WRL::ComPtr<ID3D12Device14> pD3dDevice;

	if constexpr (EnableDebugLayer) {
		Microsoft::WRL::ComPtr<ID3D12Debug> pDebugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)));
		pDebugController->EnableDebugLayer();
	}

	ThrowIfFailed(D3D12CreateDevice(HardwareAdapter(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(pD3dDevice.GetAddressOf())));

	return pD3dDevice;
}
