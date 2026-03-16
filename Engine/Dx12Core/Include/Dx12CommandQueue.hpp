#pragma once
/*
* Author: Thomas Gibson
* Created: Feb 13, 2026
*
*/

#include "dx12.hpp"
#include <vector>
namespace Dx12Framework {
	class Dx12Core_API Dx12CommandQueue {
	public:
		const D3D12_COMMAND_LIST_TYPE type;
		const Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQueue;
		const Microsoft::WRL::ComPtr<ID3D12Fence1> pFence;

		Dx12CommandQueue(ID3D12Device14* device, D3D12_COMMAND_LIST_TYPE type);
		~Dx12CommandQueue();

		Dx12CommandQueue(Dx12CommandQueue&) = delete;
		Dx12CommandQueue& operator = (const Dx12CommandQueue&) = delete;

		UINT64 Signal() const;

		void CPUWaitForFence(UINT64 value) const;

		void GPUWaitForCommandQueue(const Dx12CommandQueue& other, UINT64 value = 0llu) const;

		void Flush() const { CPUWaitForFence(Signal()); }

		UINT64 CurrentFenceValue() const { return fenceValue; }

		UINT64 ExecuteCommandLists(UINT numCommandLists, ID3D12CommandList* const* ppCommandLists) const;

		template <typename... Args>
		UINT64 ExecuteCommandLists(Args*... commandLists) const {
			/*static_assert((std::is_convertible_v<Args, ID3D12CommandList*> && ...),
						  "All arguments must be convertible to ID3D12CommandList* (pass pointers, not addresses).");*/


			ID3D12CommandList* ppCommandLists[] = { commandLists... };
			return ExecuteCommandLists(static_cast<UINT>(sizeof...(Args)), ppCommandLists);
		}

	private:
		const HANDLE eventHandle;
		mutable UINT64 fenceValue = 0;

		static Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateCommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
		static Microsoft::WRL::ComPtr<ID3D12Fence1> CreateFence(ID3D12Device* device);
	};
}