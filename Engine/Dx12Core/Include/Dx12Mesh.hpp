#pragma once


#include "Dx12DeviceContext.hpp"
#include "Dx12Frame.hpp"

#include <vector>

namespace Dx12Framework {

	template<typename T_Vertex>
	class Dx12Mesh {
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource2> pDxVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexView;
		D3D12_INDEX_BUFFER_VIEW indexView;
	public:

		Dx12Mesh() = default;
		~Dx12Mesh() = default;

		Dx12Mesh(const Dx12Framework::Dx12DeviceContext& dc, std::vector<T_Vertex> vertices, std::span<const uint32_t> indices) {

			Microsoft::WRL::ComPtr<ID3D12Resource2> pDxStagingBuffer; // used to stage vertex data

			const size_t vertexSizeBytes = (vertices.size() * sizeof(T_Vertex));

			const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexSizeBytes + indices.size_bytes());
			constexpr D3D12_CLEAR_VALUE* NO_CLEAR = nullptr;


			// Create Staging Buffer
			{
				const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

				ThrowIfFailed(dc->CreateCommittedResource(
					&heapProps, D3D12_HEAP_FLAG_NONE,
					&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
					NO_CLEAR,
					IID_PPV_ARGS(pDxStagingBuffer.GetAddressOf())));
			}

			// Build Static Vertex Buffer
			{
				const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

				ThrowIfFailed(dc->CreateCommittedResource(
					&heapProps, D3D12_HEAP_FLAG_NONE,
					&bufferDesc, D3D12_RESOURCE_STATE_COMMON,
					NO_CLEAR,
					IID_PPV_ARGS(pDxVertexBuffer.GetAddressOf())));
			}

			// Write triangle data into staging buffer
			{
				constexpr UINT Subresource= 0U;		// Default sub-resource
				constexpr D3D12_RANGE NoRead = {};	// Only interested in writing
				void* pMappedData;
				pDxStagingBuffer->Map(Subresource, &NoRead, &pMappedData);

				memcpy(pMappedData, vertices.data(), vertexSizeBytes);

				uint8_t* pOffsetMemory = static_cast<uint8_t*>(pMappedData) + vertexSizeBytes;
				memcpy(pOffsetMemory, indices.data(), indices.size_bytes());

				pDxStagingBuffer->Unmap(Subresource, &NoRead);
			}

			// Set up vertex buffer view
			vertexView.BufferLocation = pDxVertexBuffer->GetGPUVirtualAddress();
			vertexView.SizeInBytes = vertexSizeBytes;
			vertexView.StrideInBytes = sizeof(T_Vertex);

			// Set up index buffer view
			indexView.BufferLocation = pDxVertexBuffer->GetGPUVirtualAddress() + vertexSizeBytes;
			indexView.SizeInBytes = indices.size_bytes();
			indexView.Format = DXGI_FORMAT_R32_UINT;


			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> pDx12CopyList;
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pDx12CopyAlloc;

			dc->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&pDx12CopyAlloc));
			dc->CreateCommandList1(0U, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pDx12CopyList));

			// Copy to default buffer
			{
				pDx12CopyAlloc->Reset();
				pDx12CopyList->Reset(pDx12CopyAlloc.Get(), nullptr);
				pDx12CopyList->CopyBufferRegion(pDxVertexBuffer.Get(), 0U, pDxStagingBuffer.Get(), 0U, vertexSizeBytes + indices.size_bytes());
				pDx12CopyList->Close();

				//ID3D12CommandList* lists[] = { pDx12CopyList.Get() };
				const UINT64 copyFence = dc.dxCopyQueue.ExecuteCommandLists(pDx12CopyList.Get());

				dc.dxCopyQueue.CPUWaitForFence(copyFence);
			}
		}

		Dx12Mesh(const Dx12Framework::Dx12DeviceContext& dc, std::span<T_Vertex> vertices, std::span<const uint32_t> indices) {

			Microsoft::WRL::ComPtr<ID3D12Resource2> pDxStagingBuffer; // used to stage vertex data
			const CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size_bytes() + indices.size_bytes());
			constexpr D3D12_CLEAR_VALUE* NO_CLEAR = nullptr;


			// Create Staging Buffer
			{
				const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

				ThrowIfFailed(dc->CreateCommittedResource(
					&heapProps, D3D12_HEAP_FLAG_NONE,
					&bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
					NO_CLEAR,
					IID_PPV_ARGS(pDxStagingBuffer.GetAddressOf())));
			}

			// Build Static Vertex Buffer
			{
				const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

				ThrowIfFailed(dc->CreateCommittedResource(
					&heapProps, D3D12_HEAP_FLAG_NONE,
					&bufferDesc, D3D12_RESOURCE_STATE_COMMON,
					NO_CLEAR,
					IID_PPV_ARGS(pDxVertexBuffer.GetAddressOf())));
			}

			// Write triangle data into staging buffer
			{
				constexpr UINT Subresource= 0U;		// Default sub-resource
				constexpr D3D12_RANGE NoRead = {};	// Only interested in writing
				void* pMappedData;
				pDxStagingBuffer->Map(Subresource, &NoRead, &pMappedData);

				memcpy(pMappedData, vertices.data(), vertices.size_bytes());

				uint8_t* pOffsetMemory = static_cast<uint8_t*>(pMappedData) + vertices.size_bytes();
				memcpy(pOffsetMemory, indices.data(), indices.size_bytes());

				pDxStagingBuffer->Unmap(Subresource, &NoRead);
			}

			// Set up vertex buffer view
			vertexView.BufferLocation = pDxVertexBuffer->GetGPUVirtualAddress();
			vertexView.SizeInBytes = vertices.size_bytes();
			vertexView.StrideInBytes = sizeof(T_Vertex);

			// Set up index buffer view
			indexView.BufferLocation = pDxVertexBuffer->GetGPUVirtualAddress() + vertices.size_bytes();
			indexView.SizeInBytes = indices.size_bytes();
			indexView.Format = DXGI_FORMAT_R32_UINT;


			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> pDx12CopyList;
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pDx12CopyAlloc;

			dc->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&pDx12CopyAlloc));
			dc->CreateCommandList1(0U, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pDx12CopyList));

			// Copy to default buffer
			{
				pDx12CopyAlloc->Reset();
				pDx12CopyList->Reset(pDx12CopyAlloc.Get(), nullptr);
				pDx12CopyList->CopyBufferRegion(pDxVertexBuffer.Get(), 0U, pDxStagingBuffer.Get(), 0U, vertices.size_bytes() + indices.size_bytes());
				pDx12CopyList->Close();

				const UINT64 copyFence = dc.dxCopyQueue.ExecuteCommandLists(pDx12CopyList.Get());

				dc.dxCopyQueue.CPUWaitForFence(copyFence);
			}
		}

		void Draw(const Dx12Framework::Dx12Frame& frame, D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) const {
			// Set triangle data
			constexpr UINT VertexBufferCount= 1U;
			frame->IASetVertexBuffers(0U, VertexBufferCount, &vertexView);
			frame->IASetIndexBuffer(&indexView);

			// Interpret vertex data as a triangle list
			frame->IASetPrimitiveTopology(topology);

			// Draw!!!
			const UINT IndexCount = indexView.SizeInBytes / 4U;
			constexpr UINT InstanceCount = 1U;
			frame->DrawIndexedInstanced(IndexCount, InstanceCount, 0U, 0U, 0U);
		}
	};
}