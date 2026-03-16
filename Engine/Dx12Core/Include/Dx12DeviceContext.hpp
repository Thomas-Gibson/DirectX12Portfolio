#pragma once

#include "dx12.hpp"
#include "Dx12CommandQueue.hpp"

namespace Dx12Framework {

	class Dx12Core_API Dx12DeviceContext {
	private:

#ifdef _DEBUG
		static constexpr bool EnableDebugLayer = true;
#else 
		static constexpr bool EnableDebugLayer = false;
#endif // _DEBUG

		const Microsoft::WRL::ComPtr<IDXGIFactory7> pDxgiFactory;
		const Microsoft::WRL::ComPtr<IDXGIAdapter4> pDxgiHardwareAdapter;
		const Microsoft::WRL::ComPtr<ID3D12Device14> pDxDevice;

	public:
		const Dx12CommandQueue dxDirectQueue;
		const Dx12CommandQueue dxCopyQueue;

		Dx12DeviceContext();
		~Dx12DeviceContext() = default;

		Dx12DeviceContext(const Dx12DeviceContext&) = delete;
		Dx12DeviceContext& operator = (const Dx12DeviceContext&) = delete;

		inline IDXGIFactory7* Factory() const { return pDxgiFactory.Get(); }
		inline IDXGIAdapter4* HardwareAdapter() const { return pDxgiHardwareAdapter.Get(); }
		inline ID3D12Device14* Device() const { return pDxDevice.Get(); };

		ID3D12Device14* operator -> () const { return Device(); }

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory7> InitFactory();
		Microsoft::WRL::ComPtr<IDXGIAdapter4> InitHardwareAdapter();
		Microsoft::WRL::ComPtr<ID3D12Device14> InitDevice();
	};
}