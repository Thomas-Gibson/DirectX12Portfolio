#pragma once
/*
* Author: Thomas Gibson
* Date: Feb 13, 2026
*/

#include <span>
#include <system_error>
#include <memory>

// Windows & Direct3D
#include <Windows.h>		// Windows API
#include <dxgi1_6.h>		// DirectX Graphics Infrastructure (DXGI) header
#include <d3d12.h>			// Main DirectX 12 header
#include <wrl/client.h>		// Microsoft::WRL::ComPtr

#include <dxgidebug.h>

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		throw std::system_error(hr, std::system_category(), "HRESULT failure");
	}
}

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib") // For DXGIGetDebugInterface1

//#ifdef Dx12Core_EXPORT
//#define Dx12Core_API __declspec(dllexport)
//#else
//#define Dx12Core_API __declspec(dllimport)
//#endif // Dx12Core_EXPORT

#define Dx12Core_API
