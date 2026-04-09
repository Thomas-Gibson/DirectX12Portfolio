#include "Dx12Camera.hpp"
#include "Keyboard.hpp"

namespace Dx12Framework {
	class Dx12FlyViewCameraController : public Dx12Camera {
	private:
		// using XMFLOAT4 to align w/ Dx12Camera data.
		DirectX::XMFLOAT4 position = { 0.0f, 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 forward = { 0.0f, 0.0f, 1.0f, 0.0f };

	public:

		Dx12FlyViewCameraController(DirectX::XMFLOAT3 defaultPosition = {}, DirectX::XMFLOAT3 defaultFacing = { 0,0,1 }) :
			position({ defaultPosition.x, defaultPosition.y, defaultPosition.z, 0 }),
			forward({ defaultFacing.x, defaultFacing.y, defaultFacing.z, 0 })
		{
			LookTo(position, forward);
		}

		void SetPosition(DirectX::XMFLOAT3 pos) {
			position = { pos.x, pos.y, pos.z, 0 };
			LookTo(position, forward);
		}

		void SetPosition(DirectX::FXMVECTOR pos) {
			DirectX::XMStoreFloat4(&position, pos);
			LookTo(position, forward);
		}

		void SetPosition(float x, float y, float z) {
			position = { x, y, z, 0 };
			LookTo(position, forward);
		}

		void SetForward(DirectX::XMFLOAT3 pos) {
			forward = { pos.x, pos.y, pos.z, 0 };
			LookTo(position, forward);
		}

		void XM_CALLCONV SetForward(DirectX::FXMVECTOR pos) {
			DirectX::XMStoreFloat4(&forward, pos);
			LookTo(position, forward);
		}

		void SetForward(float x, float y, float z) {
			forward = { x, y, z, 0 };
			LookTo(position, forward);
		}

		void Update(MouseState& m, Keyboard& k) {
			static bool M3JustPressed = false;
			static bool IsMouseLocked = false;

			const bool M3 = m.middleButton;
			if (!M3) {
				M3JustPressed = false;

				if (IsMouseLocked) {
					IsMouseLocked = false;
					ShowCursor(TRUE); // show cursor when not navigating
				}

				return;
			}


			if (!IsMouseLocked) {
				IsMouseLocked = true;
				ShowCursor(FALSE); // hides cursor while navigating
			}


			static const HWND hwnd = FindWindow("DirectX12Window", NULL);

			// finding center of the window
			RECT cr;
			GetClientRect(hwnd, &cr);
			POINT center = { (cr.right - cr.left) / 2, (cr.bottom - cr.top) / 2 };
			ClientToScreen(hwnd, &center);


			// current mouse position
			POINT mouse = {};
			GetCursorPos(&mouse);

			// compute mouse delta
			const float deltaX = static_cast<float>(mouse.x - center.x);
			const float deltaY = static_cast<float>(mouse.y - center.y);

			// "lock" mouse to center for next frame
			SetCursorPos(center.x, center.y);

			const bool W = k.IsKeyPressed('W');
			const bool A = k.IsKeyPressed('A');
			const bool S = k.IsKeyPressed('S');
			const bool D = k.IsKeyPressed('D');
			const bool Space = k.IsKeyPressed(VK_SPACE);
			const bool LShift = k.IsKeyPressed(VK_LSHIFT);

			DirectX::XMVECTOR pos = DirectX::XMLoadFloat4(&position);
			DirectX::XMVECTOR lookDir = DirectX::XMLoadFloat4(&forward);
			DirectX::XMVECTOR worldUp = { 0.0f, 1.0f, 0.0f, 0.0f };

			// 2. Handle Rotation
			float sensitivity = 0.0015f;
			float yawDelta = deltaX * sensitivity;
			float pitchDelta = deltaY * sensitivity;

			// Rotate around World Up (Yaw)
			lookDir = DirectX::XMVector3TransformNormal(lookDir, DirectX::XMMatrixRotationY(yawDelta));

			// Calculate Right axis AFTER yaw for clean Pitch
			DirectX::XMVECTOR rightAxis = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, lookDir));

			// Rotate around Right axis (Pitch) with your clamp logic
			lookDir = DirectX::XMVector3TransformNormal(lookDir, DirectX::XMMatrixRotationAxis(rightAxis, pitchDelta));

			// Safety Normalize
			lookDir = DirectX::XMVector3Normalize(lookDir);

			constexpr float SPEED = 0.05f; // Adjust move speed here

			if (W) pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(lookDir, SPEED));
			if (S) pos = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(lookDir, SPEED));
			if (D) pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(rightAxis, SPEED));
			if (A) pos = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(rightAxis, SPEED));
			if (Space && !LShift) pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(worldUp, SPEED));
			if (Space && LShift) pos = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(worldUp, SPEED));
			DirectX::XMStoreFloat4(&forward, lookDir);
			DirectX::XMStoreFloat4(&position, pos);

			LookTo(pos, lookDir);
		}
	};
}