#include "Dx12TexturesApplication.hpp"
#include <DirectXColors.h>
#include <WICTextureLoader.h>
//#include <filesystem>
//#include "ModuleUtils.hpp"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

extern "C" __declspec(dllexport)
Dx12Framework::Dx12Application* GetApplication(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) {
	return new Dx12TexturesApplication(dc, imgui);
}

Dx12TexturesApplication::Dx12TexturesApplication(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) :
	Dx12Application(dc, imgui),
	texturePipeline(dc),
	square(dc, squareVertices, squareIndices) // 'Dx12Framework::Dx12Mesh<Dx12TexturesApplication::SquareVertex>::Dx12Mesh': no overloaded function could convert all the argument types
{
	InitConstantBuffer();
	InitTexture();

	// Set matrices
	{
		worldMat = DirectX::XMMatrixIdentity();

		const XMVECTOR CameraPos = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
		const XMVECTOR FocusPoint = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		const XMVECTOR UpDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		viewMat = DirectX::XMMatrixLookAtLH(CameraPos, FocusPoint, UpDirection);

		constexpr float FOV = XMConvertToRadians(45.0f);
		constexpr float AspectRatio = 16.0f / 9.0f;
		constexpr float NearZ = 0.1f;
		constexpr float FarZ = 1000.0f;
		projectionMat = DirectX::XMMatrixPerspectiveFovLH(FOV, AspectRatio, NearZ, FarZ);
	}

	const XMMATRIX worldViewProjection = worldMat * viewMat * projectionMat;
	memcpy(pConstantBufferMemory, &worldViewProjection, sizeof(XMMATRIX));
}

void Dx12TexturesApplication::InitTexture() {
	// Create SRV Descriptor Heap
	constexpr D3D12_DESCRIPTOR_HEAP_DESC SRVDesc = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = 1U,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		.NodeMask = 1U
	};

	ThrowIfFailed(dc->CreateDescriptorHeap(&SRVDesc, IID_PPV_ARGS(pSRVHeap.GetAddressOf())));

	// Create Texture Resource
	// https://github.com/microsoft/DirectXTK12/wiki/WICTextureLoader#examples

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> pDx12CopyList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pDx12CopyAlloc;

	dc->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&pDx12CopyAlloc));
	dc->CreateCommandList1(0U, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pDx12CopyList));


	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA subresource;


	//ComPtr<ID3D12Resource> pTempTexture;

	ComPtr<ID3D12Resource> pTempTexture;
	std::filesystem::path texPath = GetPluginAssetRoot() / "icon.ico";
	std::wstring texW = texPath.wstring();

	// Call the WIC loader with the full path
	ThrowIfFailed(
		LoadWICTextureFromFile(dc.Device(), texW.c_str(), pTempTexture.GetAddressOf(),
		decodedData, subresource));

	pTempTexture.As(&pTexture);

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTexture.Get(), 0, 1);

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

	auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	// Create the GPU upload buffer.
	ComPtr<ID3D12Resource> uploadRes;
	ThrowIfFailed(
		dc->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadRes.GetAddressOf())));

	pDx12CopyAlloc->Reset();
	pDx12CopyList->Reset(pDx12CopyAlloc.Get(), nullptr);

	UpdateSubresources(pDx12CopyList.Get(), pTexture.Get(), uploadRes.Get(),
					   0, 0, 1, &subresource);

	ThrowIfFailed(pDx12CopyList->Close());
	dc.dxCopyQueue.ExecuteCommandLists(pDx12CopyList.Get());

	dc.dxCopyQueue.Flush();

	dc->CreateShaderResourceView(pTexture.Get(), nullptr, pSRVHeap->GetCPUDescriptorHandleForHeapStart());
}

void Dx12TexturesApplication::InitConstantBuffer() {
	// Build constant Buffer 
	const CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(256);
	constexpr D3D12_CLEAR_VALUE* NO_CLEAR = nullptr;
	const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

	ThrowIfFailed(dc->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
		NO_CLEAR,
		IID_PPV_ARGS(pDxConstantBuffer.GetAddressOf())));

	pDxConstantBuffer->Map(0U, nullptr, &pConstantBufferMemory);


}

void Dx12TexturesApplication::Update(float timeDelta) {

}

void Dx12TexturesApplication::Render(const Dx12Framework::Dx12Frame& frame) {
	static bool transitioned = false;

	if (!transitioned) {
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		frame->ResourceBarrier(1, &barrier);
		transitioned = true;
	}


	frame.SetDefaultViewport();
	frame.Clear(Colors::CornflowerBlue);

	texturePipeline.Bind(frame);

	UINT rootIndex = 0U;
	frame->SetGraphicsRootConstantBufferView(rootIndex++, pDxConstantBuffer->GetGPUVirtualAddress());

	ID3D12DescriptorHeap* descriptorHeaps[] = { pSRVHeap.Get() };
	frame->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	frame->SetGraphicsRootDescriptorTable(rootIndex, pSRVHeap->GetGPUDescriptorHandleForHeapStart());

	square.Draw(frame);
}
