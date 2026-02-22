#pragma once
/*
* Author: Thomas Gibson
* Created: Feb 14, 2026
*
* This project will: show the process of creating a PSO, Root Signature, and rendering a triangle.
*
* Dx12Framework Library:
*	Before starting this project I created a shared library, Dx12Framework.
*	This library will grow as I create more projects, and will contain common code that can be reused across projects.
*	
*	Key components for this project:
*		Dx12Framework::Dx12Application
*		Dx12Framework::Dx12DeviceContext
*		Dx12Framework::Dx12CommandQueue
*		Dx12Framework::Dx12RenderWindow
*		Dx12Framework::Dx12Frame 
*/

#include <Dx12Application.hpp>
#include <d3dcompiler.h> // D3DReadFileToBlob
#include <DirectXColors.h>

class Dx12TriangleApp : public Dx12Framework::Dx12Application {
public:
	Dx12Framework::Dx12DeviceContext& dxDeviceContext;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pDxPipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> pDxRootSignature;
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxStagingBuffer;	// used to stage vertex data
	Microsoft::WRL::ComPtr<ID3D12Resource2> pDxVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW triangleView;

	struct TriangleVertex {
		DirectX::XMFLOAT2 pos;
		DirectX::XMFLOAT4 color;
	};

	Dx12TriangleApp(Dx12Framework::Dx12DeviceContext& dc);

	~Dx12TriangleApp() override = default;

	void InitPipeline();
	void LoadTriangle();

	void Update(float timeDelta) override {}
	void Render(const Dx12Framework::Dx12Frame& frame) override;
};