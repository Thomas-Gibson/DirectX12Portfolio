#include "ImGuiTestApp.hpp"
#include <DirectXColors.h>

// For Dx12ApplicationLauncher to initialize & run ImGuiTestApp
extern "C" _declspec(dllexport)
Dx12Framework::Dx12Application* GetApplication(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) {
	return new ImGuiTestApp(dc, imgui);
}

ImGuiTestApp::ImGuiTestApp(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) : Dx12Application(dc, imgui)
{
	// ImGui context is set automatically.
	// Use Dx12DeviceContext to initialize ID3D12... objects.
}

void ImGuiTestApp::Update(float timeDelta)
{
	// Update logic here
}

void ImGuiTestApp::Render(const Dx12Framework::Dx12Frame& frame)
{
	// render logic here
	ImGui::SetNextWindowSize(ImVec2(400, 300));
	ImGui::Begin("ImGuiTestApp Controls");
	
	static DirectX::XMVECTORF32 color = DirectX::Colors::CornflowerBlue;
	ImGui::ColorEdit3("Clear Color", (float*) &color);
	frame.Clear(color);
	
	ImGui::End();
}
