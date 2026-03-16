#include "@NAME@App.hpp"
#include <DirectXColors.h>

// For Dx12ApplicationLauncher to initialize & run @NAME@App
extern "C" _declspec(dllexport)
Dx12Framework::Dx12Application* GetApplication(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) {
	return new @NAME@App(dc, imgui);
}

@NAME@App::@NAME@App(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) : Dx12Application(dc, imgui)
{
	// ImGui context is set automatically.
	// Use Dx12DeviceContext to initialize ID3D12... objects.
}

void @NAME@App::Update(float timeDelta)
{
	// Update logic here
}

void @NAME@App::Render(const Dx12Framework::Dx12Frame& frame)
{
	// render logic here
	ImGui::SetNextWindowSize(ImVec2(400, 300));
	ImGui::Begin("@NAME@App Controls");
	
	static DirectX::XMVECTORF32 color = DirectX::Colors::CornflowerBlue;
	ImGui::ColorEdit3("Clear Color", (float*) &color);
	frame.Clear(color);
	
	ImGui::End();
}