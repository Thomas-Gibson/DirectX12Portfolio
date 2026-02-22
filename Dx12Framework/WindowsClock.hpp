#pragma once

#include "pch.hpp"

namespace Dx12Framework {
	class WindowsClock {
	public:

		WindowsClock() {
			QueryPerformanceFrequency(&frequency);
		}

		void Tick(float& deltaTime) {
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);

			deltaTime = static_cast<float>(now.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
			lastTime = now;
		}

		void Reset() {
			QueryPerformanceCounter(&lastTime);
		}

	private:
		LARGE_INTEGER frequency = {};
		LARGE_INTEGER lastTime = {};

		float deltaTime = 0.0f;
		double totalTime = 0.0;
	};
}