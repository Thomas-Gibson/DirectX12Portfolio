#pragma once

#include "pch.hpp"
#include "WindowsClock.hpp"
#include <iostream>
#include <format>

namespace Dx12Framework {

	struct KeyInfo {
		char keyCode;
		bool newPress;

		bool operator == (char keyCode) { return this->keyCode == keyCode; }
	};

	class WindowClass : public WNDCLASSEX {
	public:

		WindowClass(WNDPROC wndProc) :
			WNDCLASSEX({
				.cbSize				= sizeof(WNDCLASSEX),
				.style				= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
				.lpfnWndProc		= wndProc,
				.cbClsExtra			= 0U,
				.cbWndExtra			= 0U,
				.hInstance			= GetModuleHandle(nullptr),
				.hIcon				= LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1)),
				.hCursor            = LoadCursor(nullptr, IDC_ARROW),
				.hbrBackground      = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH)),
				.lpszMenuName       = nullptr,
				.lpszClassName      = L"DirectX12Window",
				.hIconSm            = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1)),
					   })
		{
			RegisterClassEx(this);
		}

		~WindowClass() {
			UnregisterClass(lpszClassName, hInstance);
		}
	};


	class WindowsApplication {
	public:
		const HWND windowHandle;

		WindowsApplication(const wchar_t* title) :
			windowHandle(CreateWindowHandle(title))
		{
			if (!windowHandle) {
				throw std::runtime_error("Failed to create window");
			}
		}

		~WindowsApplication() = default;

		void Run() {
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

		virtual bool Epoch(float timeDelta) = 0;
		virtual void OnKeyPress(KeyInfo key) = 0;
		virtual void OnKeyRelease(KeyInfo key) = 0;

		double Time() const { return time; }
		float DeltaTime() const { return deltaTime; }

		operator HWND() const { return windowHandle; }

		bool ProcessEvents() {
			MSG msg = {};
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			return msg.message != WM_QUIT;
		}

	private:
		double time = 0.0f;
		float deltaTime = 0.0f;

		static LRESULT CALLBACK InitProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			if (msg == WM_NCCREATE) {
				LPCREATESTRUCT lpCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);

				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(lpCreateStruct->lpCreateParams));
				SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(MainProc));

				return MainProc(hwnd, msg, wParam, lParam);
			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		static LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

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

		HWND CreateWindowHandle(const wchar_t* title) {
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
	};
}
