#pragma once

#include "dx12.hpp"
#include <Windows.h>
#include <windowsx.h>
#include "WindowsClock.hpp"
#include <iostream>
#include <format>

namespace Dx12Framework {

	struct KeyInfo {
		char keyCode;
		bool newPress;

		bool operator == (char keyCode) { return this->keyCode == keyCode; }
	};

	class Dx12Core_API WindowsApplication {
	public:
		const HWND windowHandle;

		WindowsApplication(const char* title);
		virtual ~WindowsApplication() = default;

		void Run();

		virtual bool Epoch(float timeDelta) = 0;
		virtual void OnKeyPress(KeyInfo key) = 0;
		virtual void OnKeyRelease(KeyInfo key) = 0;

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
