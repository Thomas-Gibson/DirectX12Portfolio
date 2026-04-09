
#include <Windows.h>
#include <DirectXMath.h>

namespace Dx12Framework {
	class Dx12Camera {
	private:
		DirectX::XMMATRIX view = {};
		DirectX::XMMATRIX projection = {};

	public:
		void XM_CALLCONV LookAt(DirectX::FXMVECTOR cameraPosition, DirectX::FXMVECTOR focusPoint) {
			view = DirectX::XMMatrixLookAtLH(cameraPosition, focusPoint, DirectX::g_XMIdentityR1);
		}

		void LookAt(DirectX::XMFLOAT4 cameraPosition, DirectX::XMFLOAT4 focusPoint) {
			LookAt(DirectX::XMLoadFloat4(&cameraPosition), DirectX::XMLoadFloat4(&focusPoint));
		}

		void XM_CALLCONV LookTo(DirectX::FXMVECTOR cameraPosition, DirectX::FXMVECTOR focusDirection) {
			view = DirectX::XMMatrixLookToLH(cameraPosition, focusDirection, DirectX::g_XMIdentityR1);
		}

		void LookTo(DirectX::XMFLOAT4 cameraPosition, DirectX::XMFLOAT4 focusDirection) {
			LookTo(DirectX::XMLoadFloat4(&cameraPosition), DirectX::XMLoadFloat4(&focusDirection));
		}

		void SetPerspectiveLens(float fovY, float aspect, float zNear, float zFar) {
			projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, zNear, zFar);
		}

		void SetOrthographicLens(float viewWidth, float viewHeight, float zn, float zf) {
			projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, zn, zf);
		}

		inline DirectX::XMMATRIX View() const { return view; }
		inline DirectX::XMMATRIX Projection() const { return projection; }
		inline DirectX::XMMATRIX ViewProjection() const { return view * projection; }
	};
}