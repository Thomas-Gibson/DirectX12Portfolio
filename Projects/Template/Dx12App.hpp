#pragma once

#include <Dx12Application.hpp>

class @NAME@App : public Dx12Framework::Dx12Application {
public:
	@NAME@App(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui);

	// Inherited via Dx12Application
	void Update(float timeDelta) override;
	void Render(const Dx12Framework::Dx12Frame& frame) override;

	// Optional virtual functions
	const wchar_t* GetApplicationName() const override { return L"@NAME@"; } // For renaming launcher window.
};