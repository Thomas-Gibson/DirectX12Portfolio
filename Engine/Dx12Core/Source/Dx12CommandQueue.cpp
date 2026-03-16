#include "Dx12CommandQueue.hpp"

Dx12Framework::Dx12CommandQueue::Dx12CommandQueue(ID3D12Device14* device, D3D12_COMMAND_LIST_TYPE type) :
	type(type),
	pCommandQueue(CreateCommandQueue(device, type)),
	pFence(CreateFence(device)),
	eventHandle(CreateEvent(nullptr, false, false, nullptr))
{
}

Dx12Framework::Dx12CommandQueue::~Dx12CommandQueue() {
	Flush();
	CloseHandle(eventHandle);
}

UINT64 Dx12Framework::Dx12CommandQueue::Signal() const {
	pCommandQueue->Signal(pFence.Get(), ++fenceValue);

	if (!pFence) {
		int a = 0;
	}
	return fenceValue;
}

void Dx12Framework::Dx12CommandQueue::CPUWaitForFence(UINT64 value) const {

	if (!pFence) {
		int a = 0;
	}

	if (pFence->GetCompletedValue() < value) {
		pFence->SetEventOnCompletion(value, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
	}

	if (!pFence) {
		int a = 0;
	}
}

void Dx12Framework::Dx12CommandQueue::GPUWaitForCommandQueue(const Dx12CommandQueue& other, UINT64 value) const {
	value = value > 0llu ? value : other.fenceValue;

	pCommandQueue->Wait(other.pFence.Get(), value);
}

UINT64 Dx12Framework::Dx12CommandQueue::ExecuteCommandLists(UINT numCommandLists, ID3D12CommandList* const* ppCommandLists) const {
	pCommandQueue->ExecuteCommandLists(numCommandLists, ppCommandLists);
	return Signal();
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> Dx12Framework::Dx12CommandQueue::CreateCommandQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
	D3D12_COMMAND_QUEUE_DESC desc = {
		.Type = type,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
	};

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
	return commandQueue;
}

Microsoft::WRL::ComPtr<ID3D12Fence1> Dx12Framework::Dx12CommandQueue::CreateFence(ID3D12Device* device)
{
	Microsoft::WRL::ComPtr<ID3D12Fence1> fence;
	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	return fence;
}

