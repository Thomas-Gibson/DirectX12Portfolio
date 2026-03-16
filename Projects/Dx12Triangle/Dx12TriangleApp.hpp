#pragma once

//#include <Dx12Application.hpp>
//
//class Dx12TriangleApp : public Dx12Framework::Dx12Application {
//public:
//	Dx12TriangleApp(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui);
//
//	// Inherited via Dx12Application
//	void Update(float timeDelta) override;
//	void Render(const Dx12Framework::Dx12Frame& frame) override;
//
//	// Optional virtual functions
//	const wchar_t* GetApplicationName() const override { return L"Dx12Triangle"; } // For renaming launcher window.
//};


#include <Dx12Application.hpp>
#include <d3dcompiler.h> // D3DReadFileToBlob
#include <DirectXColors.h>

class Dx12TriangleApp : public Dx12Framework::Dx12Application {
public:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pDxPipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> pDxRootSignature;
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxStagingBuffer;	// used to stage vertex data
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW triangleView;

	struct TriangleVertex {
		DirectX::XMFLOAT2 pos;
		DirectX::XMFLOAT4 color;
	};

	Dx12TriangleApp(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui);

	~Dx12TriangleApp() override = default;

	void InitPipeline();
	void LoadTriangle();

	void Update(float timeDelta) override {}
	void Render(const Dx12Framework::Dx12Frame& frame) override;
	const wchar_t* GetApplicationName() const override { return L"DirectX12: Triangle"; }
};