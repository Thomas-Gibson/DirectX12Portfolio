#pragma once

#include "dx12.hpp"
#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <format>
#include "WindowsClock.hpp"
#include "Keyboard.hpp"

namespace Dx12Framework {

	class WindowsApplication {
	public:
		const HWND windowHandle;
		Keyboard keyboard = {};
		MouseState mouse = {};

		WindowsApplication(const char* title);
		virtual ~WindowsApplication() = default;

		void Run();

		virtual bool Epoch(float timeDelta) = 0;

		double Time() const { return time; }
		float DeltaTime() const { return deltaTime; }

		operator HWND() const { return windowHandle; }

	private:
		class WindowClass;

		double time = 0.0f;
		float deltaTime = 0.0f;

		static LRESULT CALLBACK InitProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND CreateWindowHandle(const char* title);
	};
}
