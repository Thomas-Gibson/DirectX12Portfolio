#include "WindowsApplication.hpp"
#include <backends/imgui_impl_win32.h>

class Dx12Framework::WindowsApplication::WindowClass : public WNDCLASSEX {
public:

	WindowClass(WNDPROC wndProc) :
		WNDCLASSEX({
			.cbSize				= sizeof(WNDCLASSEX),
			.style				= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
			.lpfnWndProc		= wndProc,
			.cbClsExtra			= 0U,
			.cbWndExtra			= 0U,
			.hInstance			= GetModuleHandle(nullptr),
			.hIcon				= LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_APPLICATION)),
			.hCursor            = LoadCursor(nullptr, IDC_ARROW),
			.hbrBackground      = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH)),
			.lpszMenuName       = nullptr,
			.lpszClassName      = TEXT("DirectX12Window"),
			.hIconSm            = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_APPLICATION)),
				   })
	{
		RegisterClassEx(this);
	}

	~WindowClass() {
		UnregisterClass(lpszClassName, hInstance);
	}
};

Dx12Framework::WindowsApplication::WindowsApplication(const char* title) :
	windowHandle(CreateWindowHandle(title))
{
	if (!windowHandle) {
		throw std::runtime_error("Failed to create window");
	}
}

void Dx12Framework::WindowsApplication::Run()
{
	ShowWindow(windowHandle, SW_SHOW);
	WindowsClock clock;
	clock.Reset();


	MSG msg = {};
	while (msg.message != WM_QUIT) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}



		if (!Epoch(deltaTime)) {
			SendMessage(windowHandle, WM_CLOSE, 0U, 0U);
		}

		clock.Tick(deltaTime);
		time += deltaTime;
		std::cout << std::format("{:.4} seconds", deltaTime) << "\r";
	}
}

LRESULT Dx12Framework::WindowsApplication::InitProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE) {
		LPCREATESTRUCT lpCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(lpCreateStruct->lpCreateParams));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(MainProc));

		return MainProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Dx12Framework::WindowsApplication::MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return true;
	}

	WindowsApplication* pApp = reinterpret_cast<WindowsApplication*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (msg) {
		case WM_KEYDOWN: {

			KeyInfo kInfo = {
				.keyCode = static_cast<char>(wParam),
				.newPress = !(lParam & (1 << 30))
			};

			pApp->OnKeyPress(kInfo);

		} break;


		case WM_KEYUP: {
			KeyInfo kInfo = {
				.keyCode = static_cast<char>(wParam),
				.newPress = false
			};

			pApp->OnKeyRelease(kInfo);
		} break;

		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

HWND Dx12Framework::WindowsApplication::CreateWindowHandle(const char* title)
{
	static WindowClass wc(InitProc);

	const HWND hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		title,
		WS_OVERLAPPEDWINDOW,			// Regular window with title bar and border
		CW_USEDEFAULT, CW_USEDEFAULT,	// Default position and size
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,	// No parent window
		nullptr,	// No menu
		wc.hInstance,
		this);

	return hwnd;
}
