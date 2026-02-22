#pragma once


class TriangleMesh {
private:
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxStagingBuffer; // used to stage vertex data
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW triangleView;

	struct TriangleVertex {
		DirectX::XMFLOAT2 pos;
		DirectX::XMFLOAT4 color;
	};

	static inline TriangleVertex triangleVertices[] = {
			{DirectX::XMFLOAT2(0.8, -0.8), DirectX::XMFLOAT4(DirectX::Colors::Blue) },
			{DirectX::XMFLOAT2(-0.8, -0.8), DirectX::XMFLOAT4(DirectX::Colors::Red) },
			{DirectX::XMFLOAT2(0.0,  0.8), DirectX::XMFLOAT4(DirectX::Colors::LimeGreen) },
	};

public:
	TriangleMesh(Dx12Framework::Dx12DeviceContext& dc) {

		const CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(triangleVertices));
		constexpr D3D12_CLEAR_VALUE* NO_CLEAR = nullptr;


		// Create Staging Buffer
		{
			const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

			ThrowIfFailed(dc->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE,
				&vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
				NO_CLEAR,
				IID_PPV_ARGS(pDxStagingBuffer.GetAddressOf())));
		}

		// Build Static Vertex Buffer
		{
			const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

			ThrowIfFailed(dc->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE,
				&vertexBufferDesc, D3D12_RESOURCE_STATE_COMMON,
				NO_CLEAR,
				IID_PPV_ARGS(pDxVertexBuffer.GetAddressOf())));
		}

		// Write triangle data into staging buffer
		{
			constexpr UINT Subresource= 0U;		// Default sub-resource
			constexpr D3D12_RANGE NoRead = {};	// Only interested in writing
			void* pMappedData;
			pDxStagingBuffer->Map(Subresource, &NoRead, &pMappedData);

			memcpy(pMappedData, triangleVertices, sizeof(triangleVertices));

			pDxStagingBuffer->Unmap(Subresource, &NoRead);
		}

		// Set up vertex buffer view
		triangleView.BufferLocation = pDxVertexBuffer->GetGPUVirtualAddress();
		triangleView.SizeInBytes = sizeof(triangleVertices);
		triangleView.StrideInBytes = sizeof(TriangleVertex);

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> pDx12CopyList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pDx12CopyAlloc;

		dc->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&pDx12CopyAlloc));
		dc->CreateCommandList1(0U, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pDx12CopyList));

		// Copy to default buffer
		{
			pDx12CopyAlloc->Reset();
			pDx12CopyList->Reset(pDx12CopyAlloc.Get(), nullptr);
			pDx12CopyList->CopyBufferRegion(pDxVertexBuffer.Get(), 0, pDxStagingBuffer.Get(), 0U, sizeof(triangleVertices));
			pDx12CopyList->Close();

			ID3D12CommandList* lists[] = { pDx12CopyList.Get() };
			const UINT64 copyFence = dc.dxCopyQueue.ExecuteCommandLists(lists);

			dc.dxCopyQueue.CPUWait(copyFence);
		}
	}

	~TriangleMesh() = default;

	void Draw(const Dx12Framework::Dx12Frame& frame) const {
		// Set triangle data
		constexpr UINT VertexBufferCount= 1U;
		frame->IASetVertexBuffers(0U, VertexBufferCount, &triangleView);

		// Interpret vertex data as a triangle list
		frame->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Draw!!!
		constexpr UINT VertexCount = 3U;
		constexpr UINT InstanceCount = 1U;
		frame->DrawInstanced(VertexCount, InstanceCount, 0U, 0U);
	}
};

