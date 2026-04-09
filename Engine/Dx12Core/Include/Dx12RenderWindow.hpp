#pragma once
/*
* Author: Thomas Gibson
* Created: Feb 13, 2026
*/

#include "Dx12Frame.hpp"
#include "Dx12DeviceContext.hpp"

namespace Dx12Framework {
	class Dx12Core_API Dx12RenderWindow {
	public:
		static constexpr UINT32 FrameCount = 2;

		const Dx12CommandQueue& commandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> pDxgiSwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDxRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDxDsvHeap;

		// Frame resources
		Microsoft::WRL::ComPtr<ID3D12Resource2> pDxDsvBuffer;
		Dx12Frame frames[FrameCount];
		uint32_t frameIndex = 0U;

		Dx12RenderWindow(const Dx12DeviceContext& dx12Context, HWND windowHandle);
		Dx12Frame& NextFrame();

		void FlushFrame() {
			Dx12Frame& currentFrame = frames[pDxgiSwapChain->GetCurrentBackBufferIndex()];
			currentFrame.ready = false;
		}

	private:

		static Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(ID3D12CommandQueue* pCommandQueue, IDXGIFactory7* pFactory, HWND hwnd);
		void CreateRenderTargetViews(ID3D12Device14* pDxDevice);
	};
}