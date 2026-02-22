#include "Dx12TriangleApplication.hpp"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

extern "C" _declspec(dllexport) Dx12Framework::Dx12Application* GetApplication(Dx12Framework::Dx12DeviceContext& dc) {
	return  new Dx12TriangleApp(dc);
}

Dx12TriangleApp::Dx12TriangleApp(Dx12Framework::Dx12DeviceContext& dc) : dxDeviceContext(dc) {

	InitPipeline();
	LoadTriangle();
}


void Dx12TriangleApp::InitPipeline() {

	// Visual Studio compiles .hlsl files automatically.
	ComPtr<ID3DBlob> VertexShader, Pixelshader;

	ThrowIfFailed(D3DReadFileToBlob(L"shaders/VertexShader.cso", VertexShader.GetAddressOf()));
	ThrowIfFailed(D3DReadFileToBlob(L"shaders/PixelShader.cso", Pixelshader.GetAddressOf()));

	/*
	* Root signatures can be built manually in using D3D12SerializeRootSignature();
	* However, shader model 5.0 and up let us declare root signatures inside a shader (see VertexShader.hlsl).
	* The root signature can be extracted using the shader byte code.
	*/
	ThrowIfFailed(dxDeviceContext->CreateRootSignature(0U,
				  VertexShader->GetBufferPointer(),
				  VertexShader->GetBufferSize(),
				  IID_PPV_ARGS(pDxRootSignature.GetAddressOf())));

	// Specify the input description for the vertex shader.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{"POSITION", 0U, DXGI_FORMAT_R32G32_FLOAT, 0U, 0U, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0U},
		{"COLOR", 0U, DXGI_FORMAT_R32G32B32A32_FLOAT, 0U, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0U},
	};

	// Setup PIPELINE_STATE_STREAM_DESC
	struct {
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE rootSig;
		CD3DX12_PIPELINE_STATE_STREAM_VS vs;
		CD3DX12_PIPELINE_STATE_STREAM_PS ps;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT inputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS renderTargets;
	} TrianglePipeline;

	// Initialize TrianglePipeline
	TrianglePipeline.rootSig = pDxRootSignature.Get();
	TrianglePipeline.vs = CD3DX12_SHADER_BYTECODE(VertexShader.Get());
	TrianglePipeline.ps = CD3DX12_SHADER_BYTECODE(Pixelshader.Get());
	TrianglePipeline.inputLayout = { inputElementDescs, _countof(inputElementDescs) };

	const D3D12_RT_FORMAT_ARRAY rtvFormats = {
		.RTFormats{{DXGI_FORMAT_R8G8B8A8_UNORM}},
		.NumRenderTargets = 1U
	};

	TrianglePipeline.renderTargets = rtvFormats;


	// Build Pipeline
	const D3D12_PIPELINE_STATE_STREAM_DESC psoDesc = {
		.SizeInBytes = sizeof(TrianglePipeline),
		.pPipelineStateSubobjectStream = &TrianglePipeline
	};

	ThrowIfFailed(dxDeviceContext->CreatePipelineState(&psoDesc, IID_PPV_ARGS(pDxPipelineState.GetAddressOf())));
}

void Dx12TriangleApp::LoadTriangle() {
	// Define triangle vertices
	const TriangleVertex triangleVertices[] = {
		{XMFLOAT2(0.8, -0.8), XMFLOAT4(Colors::Blue) },
		{XMFLOAT2(-0.8, -0.8), XMFLOAT4(Colors::Red) },
		{XMFLOAT2(0.0,  0.8), XMFLOAT4(Colors::LimeGreen) },
	};

	const CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(triangleVertices));
	constexpr D3D12_CLEAR_VALUE* NO_CLEAR = nullptr;


	// Create Staging Buffer
	{
		/*
		* A D3D12_HEAP_TYPE_UPLOAD buffer is CPU and GPU accessible.
		* It's used to pass CPU memory to the GPU.
		* It's best practice for mesh data to go inside a D3D12_HEAP_TYPE_DEFAULT buffer.
		* Since the CPU cannot directly map/unmap D3D12_HEAP_TYPE_DEFAULT buffer, we build a staging buffer to facilitate uploading.
		*/

		const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

		ThrowIfFailed(dxDeviceContext->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE,
			&vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			NO_CLEAR,
			IID_PPV_ARGS(pDxStagingBuffer.GetAddressOf())));
	}


	// Build Static Vertex Buffer
	{
		/*
		* The D3D12_HEAP_TYPE_DEFAULT heap is GPU only.
		* It's preferred as GPU can access this memory much faster than a D3D12_HEAP_TYPE_UPLOAD buffer.
		*/
		const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

		ThrowIfFailed(dxDeviceContext->CreateCommittedResource(
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

		// Unmap is optional in Dx12 unlike in Dx11.
		// It's best to leave a staging buffer persistently mapped to support faster asset streaming.
		// optional:
		constexpr D3D12_RANGE* WriteRange = nullptr;
		pDxStagingBuffer->Unmap(Subresource, WriteRange);
	}


	// Set up vertex buffer view
	triangleView.BufferLocation = pDxVertexBuffer->GetGPUVirtualAddress();
	triangleView.SizeInBytes = sizeof(triangleVertices);
	triangleView.StrideInBytes = sizeof(TriangleVertex);

	// Copy to default buffer
	{
		/*
		* We use memcpy to write data to an upload heap(staging buffer).
		* For a default heap, since it's not CPU accessible, writing is facilitated by the GPU's copy engine instructed by the command list.
		* You can use a direct queue to perform the copy, but it's best to use a dedicated copy command allocator/list/queue.
		* This is so the GPU's copy engine and graphics engine can work in parallel.
		* However, D3D12_COMMAND_LIST_TYPE_COPY cannot transition resources for copying.
		* This is not a problem here since D3D12_RESOURCE_STATE_COMMON will be automatically promoted by D3D12 to D3D12_RESOURCE_STATE_COPY_DEST.
		*/

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> pDx12CopyList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pDx12CopyAlloc;

		dxDeviceContext->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&pDx12CopyAlloc));
		dxDeviceContext->CreateCommandList1(0U, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pDx12CopyList));

		pDx12CopyAlloc->Reset();
		pDx12CopyList->Reset(pDx12CopyAlloc.Get(), nullptr);
		pDx12CopyList->CopyBufferRegion(pDxVertexBuffer.Get(), 0, pDxStagingBuffer.Get(), 0U, sizeof(triangleVertices));
		pDx12CopyList->Close();


		ID3D12CommandList* lists[] = { pDx12CopyList.Get() };
		const UINT64 copyFence = dxDeviceContext.dxCopyQueue.ExecuteCommandLists(lists);

		// wait for copying to complete before proceeding to rendering
		dxDeviceContext.dxCopyQueue.Flush();
	}
}

void Dx12TriangleApp::Render(const Dx12Framework::Dx12Frame& frame) {

	const D3D12_RECT clipRect = { 0, 0, frame.Width(), frame.Height() };
	const D3D12_VIEWPORT viewport = {
		.Width = static_cast<FLOAT>(clipRect.right - clipRect.left),
		.Height = static_cast<FLOAT>(clipRect.bottom - clipRect.top),
		.MaxDepth = 1.0f
	};

	constexpr UINT ViewportCount = 1U;
	frame->RSSetViewports(ViewportCount, &viewport);
	frame->RSSetScissorRects(ViewportCount, &clipRect);

	frame.Clear(Colors::CornflowerBlue);

	// Set the triangle pipeline
	frame->SetPipelineState(pDxPipelineState.Get());
	frame->SetGraphicsRootSignature(pDxRootSignature.Get());

	// Set Input Assembler State
	constexpr UINT VertexBufferCount= 1U;
	frame->IASetVertexBuffers(0U, VertexBufferCount, &triangleView);
	frame->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw!!!
	constexpr UINT VertexCount = 3U;
	constexpr UINT InstanceCount = 1U;
	frame->DrawInstanced(VertexCount, InstanceCount, 0U, 0U);
}