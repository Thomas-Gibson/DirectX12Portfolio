#pragma once

#include <d3dx12/d3dx12.h>
#include <Dx12Application.hpp>
#include <Dx12Mesh.hpp>
#include "LightingPipeline.hpp"
#include <DirectXMath.h>
#include <Dx12FlyViewCameraController.hpp>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT2 uv;
};

template<typename T>
class ConstantBuffer {
private:
	Microsoft::WRL::ComPtr<ID3D12Resource2> uploadBuffer;
	uint8_t* pMappedMemory = nullptr;
	int frameIndex = 0;

	static constexpr size_t GetAlignedSize() {
		return (sizeof(T) + 255) & ~255;
	}

public:
	ConstantBuffer(const Dx12Framework::Dx12DeviceContext& dc) {
		const size_t alignedSize = GetAlignedSize();
		const size_t totalSize = alignedSize * 3; // Space for 3 frames

		auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

		dc->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)
		);

		// Map once, keep it mapped for the lifetime of the object (common practice)
		uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pMappedMemory));
	}

	~ConstantBuffer() {
		uploadBuffer->Unmap(0U, nullptr);
		pMappedMemory = nullptr;
	}

	void Update(T& data) {
		size_t offset = frameIndex * GetAlignedSize();
		memcpy(pMappedMemory + offset, &data, sizeof(T));

		++frameIndex %= 3;
	}

	void BindCBV(const Dx12Framework::Dx12Frame& frame) {
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = uploadBuffer->GetGPUVirtualAddress();
		gpuAddress += (frameIndex * GetAlignedSize());

		frame->SetGraphicsRootConstantBufferView(frame.nextRootIndex(), gpuAddress);
	}
};

struct CBData {
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX viewProjection;
	float time = 0;
};

class Dx12LightingApp : public Dx12Framework::Dx12Application {

	static constexpr Vertex squareVertices[] = {
		{{+1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},	// bottom-right
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} },	// bottom-left
		{{-1.0f, +1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},	// top-left
		{{+1.0f, +1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},	// top-right
	};

	static constexpr uint32_t squareIndices[] = { 0,1,2, 0,2,3 };


	LightingPipeline lightingPipeline;
	float time = 0.0f;

	Dx12Framework::Dx12FlyViewCameraController camera;
	CBData cbData;
	ConstantBuffer<CBData> cb;


public:
	Dx12Framework::Dx12Mesh<Vertex> square;
	Dx12Framework::Dx12Mesh<Dx12Framework::Dx12StandardVertex> piano;


	Dx12LightingApp(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui);

	// Inherited via Dx12Application
	void Update(float timeDelta) override;
	void Render(const Dx12Framework::Dx12Frame& frame) override;

	// Optional virtual functions
	const wchar_t* GetApplicationName() const override { return L"Dx12Lighting"; } // For renaming launcher window.
};
