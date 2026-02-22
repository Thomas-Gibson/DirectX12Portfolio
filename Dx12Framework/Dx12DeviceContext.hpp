#pragma once

#include "pch.hpp"
#include "Dx12CommandQueue.hpp"

namespace Dx12Framework {
	class Dx12DeviceContext {
	public:

#ifdef _DEBUG
		static constexpr bool EnableDebugLayer = true;
#else 
		static constexpr bool EnableDebugLayer = false;
#endif // _DEBUG


		Microsoft::WRL::ComPtr<IDXGIFactory7> pDxgiFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> pDxgiHardwareAdapter;
		Microsoft::WRL::ComPtr<ID3D12Device14> pDxDevice;
		Dx12CommandQueue dxDirectQueue;
		Dx12CommandQueue dxCopyQueue;

		Dx12DeviceContext() :
			pDxDevice(InitDevice()),
			dxDirectQueue(pDxDevice.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			dxCopyQueue(pDxDevice.Get(), D3D12_COMMAND_LIST_TYPE_COPY)
		{
		}

		ID3D12Device14* operator -> () const { return pDxDevice.Get(); }

		Microsoft::WRL::ComPtr<ID3D12Device14> InitDevice() {
			Microsoft::WRL::ComPtr<ID3D12Device14> outDevice;

			constexpr UINT dxgiFactoryFlags = EnableDebugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;
			ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pDxgiFactory)));

			if constexpr (EnableDebugLayer) {
				Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
				ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
				debugController->EnableDebugLayer();
			}

			auto GetAdapter = [this](UINT index) -> HRESULT {
				return pDxgiFactory->EnumAdapterByGpuPreference(
					index,
					DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
					IID_PPV_ARGS(pDxgiHardwareAdapter.GetAddressOf()));
			};


			for (UINT adapterIndex = 0;
				 SUCCEEDED(GetAdapter(adapterIndex));
				 adapterIndex++) {

				// Get the adapter description
				DXGI_ADAPTER_DESC3 adapterDesc = {};
				if (FAILED(pDxgiHardwareAdapter->GetDesc3(&adapterDesc))) {
					continue;
				}

				// Skip software adapters
				const bool isSoftwareAdapter = (adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) != 0;
				if (isSoftwareAdapter) {
					pDxgiHardwareAdapter.Reset();
					continue;
				}

				// Check if the adapter supports DirectX12
				const HRESULT deviceResult =
					D3D12CreateDevice(pDxgiHardwareAdapter.Get(),
									  D3D_FEATURE_LEVEL_12_0,
									  IID_PPV_ARGS(outDevice.GetAddressOf()));


				if (SUCCEEDED(deviceResult)) break;

				pDxgiHardwareAdapter.Reset();
			}

			return outDevice;
		}
	};

}