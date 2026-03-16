#include "Dx12AppLauncher.hpp"

int WinMain(_In_ HINSTANCE hInstance,
			_In_opt_ HINSTANCE hPrevInstance,
			_In_ LPSTR lpCmdLine,
			_In_ int nCmdShow) {
	try {

		SetCurrentDirectory("");
		SetDllDirectory("");

		Dx12ApplicationLauncher dx12AppLauncher;
		dx12AppLauncher.Run();
	}
	catch (const std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
	}

	return ERROR_SUCCESS;
}

int main() {
	return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLineA(), SW_SHOW);
}



