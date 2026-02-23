#pragma once
#include <d3dcompiler.h> // D3DReadFileToBlob

struct TrianglePipeline {
private:
	struct {
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE rootSig;
		CD3DX12_PIPELINE_STATE_STREAM_VS vs;
		CD3DX12_PIPELINE_STATE_STREAM_PS ps;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT inputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS renderTargets;
	} TrianglePipelineDesc;

	static constexpr D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{"POSITION", 0U, DXGI_FORMAT_R32G32_FLOAT, 0U, 0U, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0U},
		{"COLOR", 0U, DXGI_FORMAT_R32G32B32A32_FLOAT, 0U, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0U},
	};


	Microsoft::WRL::ComPtr<ID3D12PipelineState> pDxPipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> pDxRootSignature;

public:
	TrianglePipeline(Dx12Framework::Dx12DeviceContext& dc) {
		// Create Root signature
		Microsoft::WRL::ComPtr<ID3DBlob> VertexShader, Pixelshader;
		{
			ThrowIfFailed(D3DReadFileToBlob(L"shaders/Dx12ConstantBuffer_VertexShader.cso", VertexShader.GetAddressOf()));
			ThrowIfFailed(D3DReadFileToBlob(L"shaders/Dx12ConstantBuffer_PixelShader.cso", Pixelshader.GetAddressOf()));

			ThrowIfFailed(dc->CreateRootSignature(0U,
						  VertexShader->GetBufferPointer(),
						  VertexShader->GetBufferSize(),
						  IID_PPV_ARGS(pDxRootSignature.GetAddressOf())));
		}

		// Initialize TrianglePipeline
		TrianglePipelineDesc.rootSig = pDxRootSignature.Get();
		TrianglePipelineDesc.vs = CD3DX12_SHADER_BYTECODE(VertexShader.Get());
		TrianglePipelineDesc.ps = CD3DX12_SHADER_BYTECODE(Pixelshader.Get());
		TrianglePipelineDesc.inputLayout = { inputElementDescs, _countof(inputElementDescs) };

		constexpr D3D12_RT_FORMAT_ARRAY RTV_FORMATS = {
			.RTFormats{{DXGI_FORMAT_R8G8B8A8_UNORM}},
			.NumRenderTargets = 1U
		};

		TrianglePipelineDesc.renderTargets = RTV_FORMATS;


		// Build Pipeline
		const D3D12_PIPELINE_STATE_STREAM_DESC psoDesc = {
			.SizeInBytes = sizeof(TrianglePipelineDesc),
			.pPipelineStateSubobjectStream = &TrianglePipelineDesc
		};

		ThrowIfFailed(dc->CreatePipelineState(&psoDesc, IID_PPV_ARGS(pDxPipelineState.GetAddressOf())));
	}

	void Bind(const Dx12Framework::Dx12Frame& frame) const {
		frame->SetPipelineState(pDxPipelineState.Get());
		frame->SetGraphicsRootSignature(pDxRootSignature.Get());
	}
};