#pragma once
#include "Dx12.hpp"

namespace Dx12Framework {
	class Dx12Core_API Dx12Frame {
	public:
		void Begin();

		void SetDefaultViewport() const {
			constexpr UINT ViewportCount = 1U;
			pCommandList->RSSetViewports(ViewportCount, &viewport);
			pCommandList->RSSetScissorRects(ViewportCount, &clipRect);
		}

		void Clear(const FLOAT clearColor[4]) const {
			pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		}

		void End();

		ID3D12GraphicsCommandList10* GetCommandList() const { return pCommandList.Get(); }
		ID3D12GraphicsCommandList10* operator ->() const { return GetCommandList(); }

		UINT32 Width() const { return width; }
		UINT32 Height() const { return height; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource2> pDxRenderTarget;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		UINT64 Fence = 0llu;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> pCommandList;

		bool ready = false;

		UINT32 width = 0U;
		UINT32 height = 0U;

		D3D12_RECT clipRect = { };
		D3D12_VIEWPORT viewport = { };

		friend class Dx12RenderWindow;
		void Initialize(ID3D12Device14* pDevice, Microsoft::WRL::ComPtr<ID3D12Resource2> pRenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle);
	};
}