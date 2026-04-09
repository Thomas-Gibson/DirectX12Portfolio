#pragma once


#include "Dx12Frame.hpp"
#include "Dx12DeviceContext.hpp"
#include "Keyboard.hpp"
#include <imgui.h>
#include <filesystem>

namespace Dx12Framework {

	class Dx12Application {
	public:
		Keyboard* keyboard;
		MouseState* mouse;

		const Dx12Framework::Dx12DeviceContext& dc;
		Dx12Application(const Dx12Framework::Dx12DeviceContext& dc, ImGuiContext* imgui) : dc(dc) {
			if (imgui) {
				ImGui::SetCurrentContext(imgui);
			}
		};
	
		virtual ~Dx12Application() = default;

		inline std::filesystem::path GetPluginAssetRoot()
		{
			char buf[MAX_PATH];
			DWORD needed = ::GetEnvironmentVariableA("DX12_PLUGIN_ASSET_ROOT", buf, (DWORD)std::size(buf));

			if (needed > 0 && needed < std::size(buf)) {
				return std::filesystem::path(std::string(buf, buf + needed));
			}

			return "";
		}

		virtual void Update(float timeDelta) = 0;
		virtual void Render(const Dx12Framework::Dx12Frame& frame) = 0;

		virtual const wchar_t* GetApplicationName() const { return L"Dx12Framework"; };
	};
}