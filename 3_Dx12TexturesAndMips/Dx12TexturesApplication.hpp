#pragma once

#include "Dx12Application.hpp"
#include "DirectXMath.h"
#include "TexturePipeline.hpp"
#include "SquareMesh.hpp"

class Dx12TexturesApplication : public Dx12Framework::Dx12Application {
private:
	
	Dx12Framework::Dx12DeviceContext dxDeviceContext;
	TexturePipeline texturePipeline;
	SquareMesh square;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pSRVHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource2> pTexture;

	DirectX::XMMATRIX worldMat;
	DirectX::XMMATRIX viewMat;
	DirectX::XMMATRIX projectionMat;

	void* pConstantBufferMemory;
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxConstantBuffer;

public:
	Dx12TexturesApplication(Dx12Framework::Dx12DeviceContext& dc);

	void InitTexture();
	void InitConstantBuffer();

	// Inherited via Dx12Application
	void Update(float timeDelta) override;

	void Render(const Dx12Framework::Dx12Frame& frame) override;

	const wchar_t* GetApplicationName() const override { return L"DirectX12: Textures"; }
};