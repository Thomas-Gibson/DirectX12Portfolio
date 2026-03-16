#include "Dx12AppLauncher.hpp"

Dx12Framework::Dx12ApplicationLauncher::Dx12ApplicationLauncher() : WindowsApplication(TEXT("DirectX12Application Launcher")), rw(ctx, windowHandle) {

	// Init srv DescriptorHeap
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = APP_SRV_HEAP_SIZE;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (ctx->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pImGuiSrvDescriptorHeap)) != S_OK) {
		throw std::runtime_error("Failed to create srv heap");
	}

	srvAlloc.Create(ctx.Device(), pImGuiSrvDescriptorHeap.Get());

	// From ImGui DirectX12 Example
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup scaling
		float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
		style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(windowHandle);

		ImGui_ImplDX12_InitInfo init_info = {};
		init_info.Device = ctx.Device();
		init_info.CommandQueue = ctx.dxDirectQueue.pCommandQueue.Get();
		init_info.NumFramesInFlight = Dx12RenderWindow::FrameCount;
		init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
		init_info.UserData = this;

		// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
		// (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
		init_info.SrvDescriptorHeap = pImGuiSrvDescriptorHeap.Get();
		init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
			auto& allocator = reinterpret_cast<Dx12ApplicationLauncher*>(info->UserData)->srvAlloc;
			return allocator.Alloc(out_cpu_handle, out_gpu_handle);
		};
		init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
			auto& allocator = reinterpret_cast<Dx12ApplicationLauncher*>(info->UserData)->srvAlloc;
			return allocator.Free(cpu_handle, gpu_handle);
		};

		ImGui_ImplDX12_Init(&init_info);
	}

	RefreshAvailablePlugins();
}

Dx12Framework::Dx12ApplicationLauncher::~Dx12ApplicationLauncher()
{
	ctx.dxDirectQueue.Flush();

	if (pApp) delete pApp;

	if (dll) {

		FreeLibrary(dll);
		// restore previous DX12_PLUGIN_ASSET_ROOT if we saved one
		if (g_prevAssetEnvPresent) {
			SetEnvironmentVariableA("DX12_PLUGIN_ASSET_ROOT", g_prevAssetEnv.c_str());
		}
		else {
			SetEnvironmentVariableA("DX12_PLUGIN_ASSET_ROOT", nullptr); // clear
		}
	}

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
void Dx12Framework::Dx12ApplicationLauncher::LoadDll(const fs::path& path)
{
	// Find asset root
	fs::path assetRootFile = path;
	assetRootFile.replace_extension(".assetroot");

	std::string assetFolderPath;
	if (fs::exists(assetRootFile)) {
		std::ifstream file(assetRootFile);
		if (std::getline(file, assetFolderPath));
	}

	// Set Environment Variable
	bool f = ::SetEnvironmentVariableA("DX12_PLUGIN_ASSET_ROOT", assetFolderPath.c_str());

	// Load next dll
	const HMODULE nextDll = LoadLibrary(path.string().c_str());
	if (!nextDll) throw std::runtime_error("Failed to load dll.");

	// Load application getter
	Dx12Application* (*GetApplication)(const Dx12DeviceContext & dc, ImGuiContext * imgui);
	GetApplication = reinterpret_cast<decltype(GetApplication)>(::GetProcAddress(nextDll, "GetApplication"));

	if (!GetApplication) {
		::FreeLibrary(nextDll);
		throw std::runtime_error("Failed to find GetApplication.");
	}

	// Init next app
	Dx12Application* pNextApp = GetApplication(ctx, ImGui::GetCurrentContext());

	if (!pNextApp) {
		::FreeLibrary(nextDll);
		throw std::runtime_error("GetApplication failed to return valid pointer.");
	}

	// Shutdown current dll state
	if (this->pApp) {

		ctx.dxCopyQueue.Flush();
		ctx.dxDirectQueue.Flush();

		delete pApp;
		if (this->dll) FreeLibrary(this->dll);
	}

	// Update new dll state
	this->dll = nextDll;
	this->pApp = pNextApp;
}


void Dx12Framework::Dx12ApplicationLauncher::RefreshAvailablePlugins()
{
	appDlls.clear();

	fs::path exePath = fs::current_path();

	for (const auto& entry : fs::directory_iterator(exePath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".dll") {
			appDlls.push_back(entry.path().filename().string());
		}
	}
}

void Dx12Framework::Dx12ApplicationLauncher::ShowLauncherUI()
{
	ImGui::Begin("Plugin Launcher");

	if (ImGui::Button("Refresh List")) {
		RefreshAvailablePlugins();
	}

	ImGui::Separator();

	for (const std::string& dllName : appDlls) {
		if (ImGui::Selectable(dllName.c_str())) {
			fs::path full = fs::current_path() / dllName;
			nextDll = full.string();
			skipFrame = true;
		}
	}

	ImGui::End();
}

bool Dx12Framework::Dx12ApplicationLauncher::Epoch(float timeDelta)
{
	if (skipFrame) {
		ctx.dxDirectQueue.Flush();
		ctx.dxCopyQueue.Flush();
		rw.FlushFrame();
		skipFrame = false;

		if (nextDll.has_value()) {
			std::string dllPath = nextDll.value();

			LoadDll(nextDll.value());
			nextDll.reset();
		}
	}

	Dx12Frame& f = rw.NextFrame();

	if (pApp) pApp->Update(timeDelta);

	f.Begin();

	if (!pApp) {
		constexpr FLOAT clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		f.Clear(clearColor);
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (pApp) pApp->Render(f);

	ShowLauncherUI();

	// Rendering
	ImGui::Render();


	ID3D12DescriptorHeap* heaps[] = { pImGuiSrvDescriptorHeap.Get() };
	f->SetDescriptorHeaps(1, heaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), f.GetCommandList());

	f.End();

	return true;
}
