#include "Dx12RenderWindow.hpp"
using namespace Dx12Framework;


Dx12RenderWindow::Dx12RenderWindow(const Dx12DeviceContext& dx12Context, HWND windowHandle) :
	commandQueue(dx12Context.dxDirectQueue),
	pDxgiSwapChain(CreateSwapChain(commandQueue.pCommandQueue.Get(), dx12Context.Factory(), windowHandle))
{
	CreateRenderTargetViews(dx12Context.Device());

	frameIndex = pDxgiSwapChain->GetCurrentBackBufferIndex();
}

Dx12Frame& Dx12Framework::Dx12RenderWindow::NextFrame() {

	Dx12Frame& currentFrame = frames[pDxgiSwapChain->GetCurrentBackBufferIndex()];

	if (currentFrame.ready) {
		ID3D12CommandList* commandLists[] = { currentFrame.pCommandList.Get() };
		currentFrame.Fence = commandQueue.ExecuteCommandLists(currentFrame.pCommandList.Get());

		ThrowIfFailed(pDxgiSwapChain->Present(1u, 0u));
	}

	Dx12Frame& nextFrame = frames[pDxgiSwapChain->GetCurrentBackBufferIndex()];
	commandQueue.CPUWaitForFence(nextFrame.Fence);

	return nextFrame;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> Dx12Framework::Dx12RenderWindow::CreateSwapChain(ID3D12CommandQueue* pCommandQueue, IDXGIFactory7* pFactory, HWND hwnd)
{
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

void Dx12Framework::Dx12RenderWindow::CreateRenderTargetViews(ID3D12Device14* pDxDevice)
{
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

