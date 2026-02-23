#pragma once

class SquareMesh {
private:
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxStagingBuffer; // used to stage vertex data
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexView;
	D3D12_INDEX_BUFFER_VIEW indexView;

	struct SquareVertex {
		DirectX::XMFLOAT2 pos;
		DirectX::XMFLOAT2 uv;
	};

	static inline SquareVertex squareVertices[] = {
			{DirectX::XMFLOAT2(0.8, -0.8), DirectX::XMFLOAT2(1.0f, 1.0f) },
			{DirectX::XMFLOAT2(-0.8, -0.8), DirectX::XMFLOAT2(0.0f, 1.0f) },
			{DirectX::XMFLOAT2(-0.8,  0.8), DirectX::XMFLOAT2(0.0f, 0.0f) },
			{DirectX::XMFLOAT2(0.8,  0.8), DirectX::XMFLOAT2(1.0f, 0.0f) },
	};

	static constexpr UINT16 squareIndices[] = {
		0,1,2,
		0,2,3
	};

public:
	SquareMesh(Dx12Framework::Dx12DeviceContext& dc) {

		const CD3DX12_RESOURCE_DESC sqaureBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(squareVertices) + sizeof(squareIndices));
		constexpr D3D12_CLEAR_VALUE* NO_CLEAR = nullptr;


		// Create Staging Buffer
		{
			const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

			ThrowIfFailed(dc->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE,
				&sqaureBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
				NO_CLEAR,
				IID_PPV_ARGS(pDxStagingBuffer.GetAddressOf())));
		}

		// Build Static Vertex Buffer
		{
			const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

			ThrowIfFailed(dc->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE,
				&sqaureBufferDesc, D3D12_RESOURCE_STATE_COMMON,
				NO_CLEAR,
				IID_PPV_ARGS(pDxVertexBuffer.GetAddressOf())));
		}

		// Write triangle data into staging buffer
		{
			constexpr UINT Subresource= 0U;		// Default sub-resource
			constexpr D3D12_RANGE NoRead = {};	// Only interested in writing
			void* pMappedData;
			pDxStagingBuffer->Map(Subresource, &NoRead, &pMappedData);

			memcpy(pMappedData, squareVertices, sizeof(squareVertices));

			uint8_t* pMappedOffset = static_cast<uint8_t*>(pMappedData) + sizeof(squareVertices);
			memcpy(pMappedOffset, squareIndices, sizeof(squareIndices));

			pDxStagingBuffer->Unmap(Subresource, &NoRead);
		}

		// Set up vertex buffer view
		vertexView.BufferLocation = pDxVertexBuffer->GetGPUVirtualAddress();
		vertexView.SizeInBytes = sizeof(squareVertices);
		vertexView.StrideInBytes = sizeof(SquareVertex);

		// Set up index buffer view
		indexView.BufferLocation = pDxVertexBuffer->GetGPUVirtualAddress() + sizeof(squareVertices);
		indexView.Format = DXGI_FORMAT_R16_UINT;
		indexView.SizeInBytes = sizeof(squareIndices);

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> pDx12CopyList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pDx12CopyAlloc;

		dc->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&pDx12CopyAlloc));
		dc->CreateCommandList1(0U, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pDx12CopyList));

		// Copy to default buffer
		{
			pDx12CopyAlloc->Reset();
			pDx12CopyList->Reset(pDx12CopyAlloc.Get(), nullptr);
			pDx12CopyList->CopyBufferRegion(pDxVertexBuffer.Get(), 0, pDxStagingBuffer.Get(), 0U, sizeof(squareVertices) + sizeof(squareIndices));
			pDx12CopyList->Close();

			ID3D12CommandList* lists[] = { pDx12CopyList.Get() };
			const UINT64 copyFence = dc.dxCopyQueue.ExecuteCommandLists(lists);

			dc.dxCopyQueue.CPUWait(copyFence);
		}
	}

	~SquareMesh() = default;

	void Draw(const Dx12Framework::Dx12Frame& frame) const {
		// Set triangle data
		constexpr UINT VertexBufferCount= 1U;
		frame->IASetVertexBuffers(0U, VertexBufferCount, &vertexView);
		frame->IASetIndexBuffer(&indexView);

		// Interpret vertex data as a triangle list
		frame->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Draw!!!
		constexpr UINT IndexCount = 6U;
		constexpr UINT InstanceCount = 1U;
		frame->DrawIndexedInstanced(IndexCount, InstanceCount, 0U, 0U, 0U);
	}
};

