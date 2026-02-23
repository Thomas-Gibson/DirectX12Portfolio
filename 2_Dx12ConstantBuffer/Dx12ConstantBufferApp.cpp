#include "Dx12ConstantBufferApp.hpp"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

extern "C" _declspec(dllexport) Dx12Framework::Dx12Application* GetApplication(Dx12Framework::Dx12DeviceContext& dc) {
	return  new Dx12ConstantBufferApp(dc);
}

Dx12ConstantBufferApp::Dx12ConstantBufferApp(Dx12Framework::Dx12DeviceContext& dc) :
	dxDeviceContext(dc),
	pipeline(dc),
	triangle(dc)
{
	InitConstantBuffer();

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
}

void Dx12ConstantBufferApp::InitConstantBuffer()
{
	// Build constant Buffer 
	{
		const CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(256);
		constexpr D3D12_CLEAR_VALUE* NO_CLEAR = nullptr;
		const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

		ThrowIfFailed(dxDeviceContext->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE,
			&vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			NO_CLEAR,
			IID_PPV_ARGS(pDxConstantBuffer.GetAddressOf())));

		pDxConstantBuffer->Map(0U, nullptr, &pConstantBufferMemory);
	}
}

void Dx12ConstantBufferApp::Update(float timeDelta)
{
	// Apply a z-axis rotation 
	const float spin = 50 * timeDelta;
	worldMat *= XMMatrixRotationZ(XMConvertToRadians(spin));

	const XMMATRIX worldViewProjection = worldMat * viewMat * projectionMat;
	memcpy(pConstantBufferMemory, &worldViewProjection, sizeof(XMMATRIX));
}

void Dx12ConstantBufferApp::Render(const Dx12Framework::Dx12Frame& frame) {
	// bind viewport 
	frame.SetDefaultViewport();

	frame.Clear(Colors::CornflowerBlue);

	pipeline.Bind(frame);

	frame->SetGraphicsRootConstantBufferView(0U, pDxConstantBuffer->GetGPUVirtualAddress());

	triangle.Draw(frame);
}