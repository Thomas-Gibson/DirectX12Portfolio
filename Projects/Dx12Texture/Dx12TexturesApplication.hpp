#pragma once

#include "Dx12Application.hpp"
#include "DirectXMath.h"
#include "TexturePipeline.hpp"
#include "Dx12Mesh.hpp"

class Dx12TexturesApplication : public Dx12Framework::Dx12Application {
private:

	TexturePipeline texturePipeline;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pSRVHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource2> pTexture;

	DirectX::XMMATRIX worldMat;
	DirectX::XMMATRIX viewMat;
	DirectX::XMMATRIX projectionMat;

	void* pConstantBufferMemory;
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxConstantBuffer;

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

	static inline UINT32 squareIndices[] = {
		0, 1, 2,
		0, 2, 3
	};

	Dx12Framework::Dx12Mesh<SquareVertex> square;

public:
	Dx12TexturesApplication(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui);

	void InitTexture();
	void InitConstantBuffer();

	// Inherited via Dx12Application
	void Update(float timeDelta) override;

	void Render(const Dx12Framework::Dx12Frame& frame) override;

	const wchar_t* GetApplicationName() const override { return L"DirectX12: Textures"; }
};