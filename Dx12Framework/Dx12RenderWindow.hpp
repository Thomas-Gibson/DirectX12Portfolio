#pragma once
/*
* Author: Thomas Gibson
* Created: Feb 13, 2026
*/

#include "Dx12Frame.hpp"
#include "Dx12CommandQueue.hpp"

namespace Dx12Framework {
	class Dx12RenderWindow {
	public:
		static constexpr UINT32 FrameCount = 2;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> pDxgiSwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDxRtvHeap;
		Dx12CommandQueue& commandQueue;

		// Frame resources

		Dx12Frame frames[FrameCount];
		uint32_t frameIndex = 0U;

		Dx12RenderWindow(Dx12CommandQueue& queue, IDXGIFactory7* pFactory, HWND windowHandle) :
			commandQueue(queue),
			pDxgiSwapChain(CreateSwapChain(queue.pCommandQueue.Get(), pFactory, windowHandle))
		{
			Microsoft::WRL::ComPtr<ID3D12Device14> pDxDevice;
			ThrowIfFailed(queue.pCommandQueue.Get()->GetDevice(IID_PPV_ARGS(pDxDevice.GetAddressOf())));

			CreateRenderTargetViews(pDxDevice.Get());

			frameIndex = pDxgiSwapChain->GetCurrentBackBufferIndex();
		}

		Dx12Frame& NextFrame() {

			Dx12Frame& currentFrame = frames[pDxgiSwapChain->GetCurrentBackBufferIndex()];

			if (currentFrame.ready) {
				ID3D12CommandList* commandLists[] = { currentFrame.pCommandList.Get() };
				currentFrame.Fence = commandQueue.ExecuteCommandLists(commandLists);

				ThrowIfFailed(pDxgiSwapChain->Present(1u, 0u));
			}

			Dx12Frame& nextFrame = frames[pDxgiSwapChain->GetCurrentBackBufferIndex()];
			commandQueue.CPUWait(nextFrame.Fence);

			return nextFrame;
		}

		void FlushFrame() {
			Dx12Frame& currentFrame = frames[pDxgiSwapChain->GetCurrentBackBufferIndex()];
			currentFrame.ready = false;
		}

	private:

		static Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(ID3D12CommandQueue* pCommandQueue, IDXGIFactory7* pFactory, HWND hwnd) {
			constexpr DXGI_SWAP_CHAIN_DESC1 SCD = {
				.Width = 0, // Use automatic sizing
				.Height = 0,
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.Stereo = FALSE,
				.SampleDesc = { 1, 0 },
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = FrameCount,
				.Scaling = DXGI_SCALING_STRETCH,
				.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
				.Flags = 0
			};

			Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
			ThrowIfFailed(pFactory->CreateSwapChainForHwnd(
				pCommandQueue,
				hwnd,
				&SCD,
				nullptr,
				nullptr,
				&swapChain1));

			Microsoft::WRL::ComPtr<IDXGISwapChain4> pSwapChain4;
			swapChain1.As(&pSwapChain4);

			return pSwapChain4;
		}

		void CreateRenderTargetViews(ID3D12Device14* pDxDevice) {
			{
				D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
				rtvHeapDesc.NumDescriptors = FrameCount;
				rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

				ThrowIfFailed(pDxDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(pDxRtvHeap.GetAddressOf())));

				const UINT rtvDescriptorSize = pDxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = pDxRtvHeap->GetCPUDescriptorHandleForHeapStart();

				for (UINT i = 0; i < FrameCount; i++) {
					Microsoft::WRL::ComPtr<ID3D12Resource2> pRenderTarget;
					pDxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&pRenderTarget));
					pDxDevice->CreateRenderTargetView(pRenderTarget.Get(), nullptr, rtvHandle);

					frames[i].Initialize(pDxDevice, pRenderTarget.Get(), rtvHandle);
					rtvHandle.ptr += rtvDescriptorSize;
				}
			}
		}
	};
}