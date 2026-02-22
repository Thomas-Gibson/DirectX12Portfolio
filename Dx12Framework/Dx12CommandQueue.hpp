#pragma once
/*
* Author: Thomas Gibson
* Created: Feb 13, 2026
*
*/

#include "pch.hpp"
#include <vector>
namespace Dx12Framework {
	class Dx12CommandQueue {
	public:
		const D3D12_COMMAND_LIST_TYPE type;
		const Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQueue;
		const Microsoft::WRL::ComPtr<ID3D12Fence1> pFence;

		Dx12CommandQueue(ID3D12Device14* device, D3D12_COMMAND_LIST_TYPE type) :
			type(type),
			pCommandQueue(CreateCommandQueue(device, type)),
			pFence(CreateFence(device)),
			eventHandle(CreateEvent(nullptr, false, false, nullptr))
		{
		}

		~Dx12CommandQueue() {
			Flush();
			CloseHandle(eventHandle);
		}

		UINT64 Signal() {
			pCommandQueue->Signal(pFence.Get(), ++fenceValue);
			return fenceValue;
		}

		void CPUWait(UINT64 value) const {
			if (pFence->GetCompletedValue() < value) {
				pFence->SetEventOnCompletion(value, eventHandle);
				WaitForSingleObject(eventHandle, INFINITE);
			}
		}

		void GPUWait(const Dx12CommandQueue& other, UINT64 value = 0llu) const {
			value = value > 0llu ? value : other.fenceValue;

			pCommandQueue->Wait(other.pFence.Get(), value);
		}

		void Flush() { CPUWait(Signal()); }

		UINT64 CurrentFenceValue() const { return fenceValue; }

		UINT64 ExecuteCommandLists(UINT numCommandLists, ID3D12CommandList* const* ppCommandLists) {
			pCommandQueue->ExecuteCommandLists(numCommandLists, ppCommandLists);
			return Signal();
		}

		UINT64 ExecuteCommandLists(std::span<ID3D12CommandList*> ppCommandLists) {
			return ExecuteCommandLists(static_cast<UINT>(ppCommandLists.size()), ppCommandLists.data());
		}

	private:
		const HANDLE eventHandle;
		UINT64 fenceValue = 0;

		static Microsoft::WRL::ComPtr<ID3D12CommandQueue> CreateCommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type) {
			D3D12_COMMAND_QUEUE_DESC desc = {
				.Type = type,
				.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
			};

			Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
			ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
			return commandQueue;
		}

		static Microsoft::WRL::ComPtr<ID3D12Fence1> CreateFence(ID3D12Device* device) {
			Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
			ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
			return fence;
		}
	};
}