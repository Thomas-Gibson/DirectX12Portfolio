#include "Dx12Frame.hpp"

#include <d3dx12/d3dx12.h>	// Helper library for D3D12

void Dx12Framework::Dx12Frame::Begin() {
	ready = false;

	ThrowIfFailed(pCommandAllocator->Reset());
	ThrowIfFailed(pCommandList->Reset(pCommandAllocator.Get(), nullptr));

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxRenderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	pCommandList->ResourceBarrier(1, &barrier);

	pCommandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);	// No depth for now
}

void Dx12Framework::Dx12Frame::End() {
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxRenderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	pCommandList->ResourceBarrier(1, &barrier);
	ThrowIfFailed(pCommandList->Close());

	ready = true;
}

void Dx12Framework::Dx12Frame::Initialize(ID3D12Device14* pDevice, Microsoft::WRL::ComPtr<ID3D12Resource2> pRenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle)
{
	pDxRenderTarget = pRenderTarget;
	this->rtvHandle = rtvHandle;

	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator)));
	ThrowIfFailed(pDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(pCommandList.GetAddressOf())));

	auto desc = pDxRenderTarget->GetDesc();
	width = static_cast<UINT32>(desc.Width);
	height = desc.Height;

	clipRect = {
		.right = static_cast<long>(Width()),
		.bottom = static_cast<long>(Height())
	};

	viewport = {
		.Width = static_cast<FLOAT>(clipRect.right - clipRect.left),
		.Height = static_cast<FLOAT>(clipRect.bottom - clipRect.top),
		.MaxDepth = 1.0f
	};
}
