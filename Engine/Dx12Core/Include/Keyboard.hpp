#pragma once

#include <bitset>
namespace Dx12Framework {

	class MouseState {
	public:
		int x, y;
		int dx, dy;
		bool leftButton;
		bool rightButton;
		bool middleButton;
		int wheelDelta;



	private:
	};

	class Keyboard {
		friend class WindowsApplication;

	private:
		static constexpr uint32_t KeyCount = 256;
		std::bitset<KeyCount> currentKeys;
		std::bitset<KeyCount> previousKeys;

	public:
		bool IsKeyPressed(uint8_t keyCode) const {
			return currentKeys.test(keyCode);
		}

		bool IsKeyJustPressed(uint8_t keyCode) const {
			return (currentKeys.test(keyCode) && !previousKeys.test(keyCode));
		}

	private:
		void OnKeyPress(uint8_t keyCode) {
			currentKeys.set(keyCode);
		}

		void OnKeyRelease(uint8_t keyCode) {
			currentKeys.reset(keyCode);
		}

		void Update(float deltaTime) {
			previousKeys = currentKeys;
		}
	};

}