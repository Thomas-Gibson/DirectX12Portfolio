#include <imgui.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>

#include <WindowsApplication.hpp>
#include <Dx12DeviceContext.hpp>
#include <Dx12RenderWindow.hpp>
#include <Dx12Application.hpp>

#include <DirectXColors.h>
using namespace DirectX;
using namespace Dx12Framework;

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;
#include "ExampleDescriptorHeapAllocator.hpp"

namespace Dx12Framework {
	
	class Dx12ApplicationLauncher : public WindowsApplication {
	public:
		Dx12DeviceContext ctx;
		Dx12RenderWindow rw;
		HMODULE dll = nullptr;
		Dx12Application* pApp = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pImGuiSrvDescriptorHeap;
		ExampleDescriptorHeapAllocator srvAlloc;
		static constexpr uint32_t APP_SRV_HEAP_SIZE = 64;

		std::vector<std::string> appDlls;

		bool skipFrame = false;

		Dx12ApplicationLauncher();

		~Dx12ApplicationLauncher() override;

		void LoadDll(const fs::path& path);


		void RefreshAvailablePlugins();

		std::optional<std::string> nextDll;

		void ShowLauncherUI();

		// Inherited via WindowsApplication
		bool Epoch(float timeDelta) override;		
	};
}
