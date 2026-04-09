#include "Dx12LightingApp.hpp"
#include <DirectXColors.h>
#include <Dx12AssetLoader.hpp>
#include "ModuleUtils.hpp"

// For Dx12ApplicationLauncher to initialize & run Dx12LightingApp
extern "C" _declspec(dllexport)
Dx12Framework::Dx12Application* GetApplication(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) {
	return new Dx12LightingApp(dc, imgui);
}

Dx12LightingApp::Dx12LightingApp(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) : Dx12Application(dc, imgui),
square(dc, squareVertices, squareIndices),
lightingPipeline(dc),
cb(dc)
{
	camera.SetPerspectiveLens(DirectX::XMConvertToRadians(60.0f), 16.f / 9.f, 0.1f, 1000.0f);
	camera.SetPosition(0, 0, -10);

	std::vector<Dx12Framework::Dx12StandardVertex> vertices;
	std::vector<uint32_t> indices;

	Dx12Framework::Dx12AssetLoader::LoadModel(GetPluginAssetRoot() / "Piano/model.obj", vertices, indices);
	piano = Dx12Framework::Dx12Mesh<Dx12Framework::Dx12StandardVertex>(dc, vertices, indices);
}

void Dx12LightingApp::Update(float timeDelta)
{
	camera.Update(*mouse, *keyboard);

	cbData.time += timeDelta;
	cbData.viewProjection = camera.ViewProjection();
	cbData.world = DirectX::XMMatrixIdentity();
	cbData.view = camera.Projection();
	
	cb.Update(cbData);
}

void Dx12LightingApp::Render(const Dx12Framework::Dx12Frame& frame)
{
	frame.SetDefaultViewport();

	// render logic here
	ImGui::SetNextWindowSize(ImVec2(400, 300));
	ImGui::Begin("Dx12LightingApp Controls");

	static DirectX::XMVECTORF32 color = DirectX::Colors::CornflowerBlue;
	ImGui::ColorEdit3("Clear Color", (float*)&color);
	frame.Clear(color);
	ImGui::End();


	lightingPipeline.Bind(frame);
	cb.BindCBV(frame);

	piano.Draw(frame);

}
