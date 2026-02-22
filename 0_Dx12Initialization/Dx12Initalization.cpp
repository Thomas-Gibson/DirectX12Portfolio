/*
* Project: DirectX 12 Triangle
* Author: Thomas Gibson
* Created: Feb 12, 2026
*
* This project demonstrates set up of DirectX12 application that renders a single triangle.
*
* The file is divided in sections:
* 1. Headers and linking
* 2. Window Creation
* 3. DirectX 12 Initialization
* 4. Uploading triangle data
* 5. Rendering loop
* 6. Cleanup
*/

// 1. Headers and linking
#include "resource.h"	// for the application icon
#include <system_error>

#include <Windows.h>		// Windows API
#include <dxgi1_6.h>		// DirectX Graphics Infrastructure (DXGI) header
#include <d3d12.h>			// Main DirectX 12 header
#include <wrl/client.h>		// Microsoft::WRL::ComPtr
using Microsoft::WRL::ComPtr;


#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXColors.h>

// Forward declaration of window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG
constexpr bool EnableDebugLayer = true;
#else 
constexpr bool EnableDebugLayer = false;
#endif // _DEBUG

void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		throw std::system_error(hr, std::system_category(), "HRESULT failure");
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_ LPSTR lpCmdLine,
				   _In_ int nCmdShow) {

	// 2. Window Creation

	// Define and register the window class
	const WNDCLASSEX wc = {
		.cbSize				= sizeof(WNDCLASSEX),
		.style				= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, // Redraw on horizontal or vertical resize, and receive double-click messages
		.lpfnWndProc		= WindowProc,
		.cbClsExtra			= 0U,
		.cbWndExtra			= 0U,
		.hInstance			= hInstance,
		.hIcon				= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)),
		.hCursor            = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground      = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH)),
		.lpszMenuName       = nullptr,
		.lpszClassName      = L"DirectX12Window",
		.hIconSm            = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)),
	};

	if (!RegisterClassEx(&wc)) {
		MessageBox(nullptr, L"Failed to register window class.", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}


	const HWND hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		L"DirectX12 Initialization",
		WS_OVERLAPPEDWINDOW,	// Regular window with title bar and border
		CW_USEDEFAULT, CW_USEDEFAULT,	// Default position and size
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,	// No parent window
		nullptr,	// No menu
		hInstance,
		nullptr);

	if (!hwnd) {
		MessageBox(nullptr, L"Failed to create window.", L"Error", MB_OK | MB_ICONERROR);
		UnregisterClassW(wc.lpszClassName, hInstance);
		return -1;
	}

	// 3 DirectX 12 Initialization

	// DXGI objects
	ComPtr<IDXGIFactory7> pDxgiFactory;
	ComPtr<IDXGIAdapter4> pDxgiHardwareAdapter;
	ComPtr<IDXGISwapChain4> pDxgiSwapChain;

	// DirectX 12 objects
	ComPtr<ID3D12Device9> pDxDevice;
	ComPtr<ID3D12CommandQueue> pDxCommandQueue;
	ComPtr<ID3D12CommandAllocator> pDxCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList7> pDxCommandList;

	// Synchronization objects
	ComPtr<ID3D12Fence1> pDxFence;
	HANDLE fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	UINT64 fenceValue = 0;

	// Render target objects
	constexpr UINT FrameCount = 2;
	ComPtr<ID3D12Resource2> pDxRenderTargets[FrameCount];
	ComPtr<ID3D12DescriptorHeap> pDxRtvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[FrameCount];
	UINT frameIndex = 0;

	try {
		// 3.0 Enable Debug Layer
		if constexpr (EnableDebugLayer) {
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}

		// 3.1 Create DXGI Factory
		constexpr UINT dxgiFactoryFlags = EnableDebugLayer ? DXGI_CREATE_FACTORY_DEBUG : 0;
		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(pDxgiFactory.GetAddressOf())));

		// 3.2 Select Hardware Adapter & Device Creation
		auto GetAdapter = [&](UINT index) -> HRESULT {
			return pDxgiFactory->EnumAdapterByGpuPreference(
				index,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(pDxgiHardwareAdapter.GetAddressOf()));
		};

		UINT adapterIndex = 0;
		while (SUCCEEDED(GetAdapter(adapterIndex++))) {

			// Get the adapter description
			DXGI_ADAPTER_DESC3 adapterDesc = {};
			if (FAILED(pDxgiHardwareAdapter->GetDesc3(&adapterDesc))) {
				continue;
			}

			// Skip software adapters
			const bool isSoftwareAdapter = (adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) != 0;
			if (isSoftwareAdapter) {
				pDxgiHardwareAdapter.Reset();
				continue;
			}

			// Check if the adapter supports DirectX12
			const HRESULT deviceResult =
				D3D12CreateDevice(pDxgiHardwareAdapter.Get(),
								  D3D_FEATURE_LEVEL_12_0,
								  IID_PPV_ARGS(pDxDevice.GetAddressOf()));


			if (SUCCEEDED(deviceResult)) break;

			pDxgiHardwareAdapter.Reset();
		}

		// 3.3 Backup: Try WARP adapter if no hardware adapter was found
		if (!pDxgiHardwareAdapter) {
			pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(pDxgiHardwareAdapter.GetAddressOf()));
			const HRESULT deviceResult = D3D12CreateDevice(pDxgiHardwareAdapter.Get(),
														   D3D_FEATURE_LEVEL_12_0,
														   IID_PPV_ARGS(pDxDevice.GetAddressOf()));

			ThrowIfFailed(deviceResult);
		}


		// 3.4 Create Command Objects
		constexpr D3D12_COMMAND_LIST_TYPE DirectType = D3D12_COMMAND_LIST_TYPE_DIRECT;

		// Command Allocator
		ThrowIfFailed(pDxDevice->CreateCommandAllocator(DirectType, IID_PPV_ARGS(pDxCommandAllocator.GetAddressOf())));

		// Command List
		ThrowIfFailed(pDxDevice->CreateCommandList1(0, DirectType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(pDxCommandList.GetAddressOf())));

		// Create command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = { DirectType, D3D12_COMMAND_QUEUE_PRIORITY_HIGH };
		ThrowIfFailed(pDxDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(pDxCommandQueue.GetAddressOf())));


		// 3.5 Create fence
		ThrowIfFailed(pDxDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(pDxFence.GetAddressOf())));

		// 3.6 Create Swap Chain
		constexpr DXGI_SWAP_CHAIN_DESC1 SCD = {
			.Width = 0, // Use automatic sizing
			.Height = 0,
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.Stereo = FALSE,
			.SampleDesc = { 1, 0 },
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = FrameCount,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = 0
		};

		// CreateSwapChainForHwnd requires a IDXGISwapChain1, we'll cast it later to IDXGISwapChain4
		ComPtr<IDXGISwapChain1> swapChain1;
		ThrowIfFailed(pDxgiFactory->CreateSwapChainForHwnd(
			pDxCommandQueue.Get(),
			hwnd,
			&SCD,
			nullptr,
			nullptr,
			&swapChain1));

		swapChain1.As(&pDxgiSwapChain);

		// 3.7 Create Render Target Views
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = FrameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			ThrowIfFailed(pDxDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(pDxRtvHeap.GetAddressOf())));

			const UINT rtvDescriptorSize = pDxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = pDxRtvHeap->GetCPUDescriptorHandleForHeapStart();

			for (UINT i = 0; i < FrameCount; i++) {
				ThrowIfFailed(pDxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&pDxRenderTargets[i])));

				pDxDevice->CreateRenderTargetView(pDxRenderTargets[i].Get(), nullptr, rtvHandle);
				rtvHandles[i] = rtvHandle;
				rtvHandle.ptr += rtvDescriptorSize;
			}
		}

		ShowWindow(hwnd, nCmdShow);

		MSG msg = {};
		while (msg.message != WM_QUIT) {
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			frameIndex = pDxgiSwapChain->GetCurrentBackBufferIndex();


			ThrowIfFailed(pDxCommandAllocator->Reset());
			ThrowIfFailed(pDxCommandList->Reset(pDxCommandAllocator.Get(), nullptr));

			D3D12_RESOURCE_BARRIER barrier = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION };

			barrier.Transition.pResource = pDxRenderTargets[frameIndex].Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			pDxCommandList->ResourceBarrier(1, &barrier);

			pDxCommandList->ClearRenderTargetView(rtvHandles[frameIndex], DirectX::Colors::CornflowerBlue, 0, nullptr);

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			pDxCommandList->ResourceBarrier(1, &barrier);

			ThrowIfFailed(pDxCommandList->Close());

			ID3D12CommandList* commandLists[] = { pDxCommandList.Get() };

			pDxCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

			ThrowIfFailed(pDxgiSwapChain->Present(1, 0));

			// flush command queue 
			ThrowIfFailed(pDxCommandQueue->Signal(pDxFence.Get(), ++fenceValue));
			if (pDxFence->GetCompletedValue() < fenceValue) {
				ThrowIfFailed(pDxFence->SetEventOnCompletion(fenceValue, fenceEvent));
				WaitForSingleObject(fenceEvent, INFINITE);
			}
		}
	}
	catch (const std::exception& ex) {
		MessageBoxA(nullptr, ex.what(), "Error", MB_OK | MB_ICONERROR);
	}

	UnregisterClassW(wc.lpszClassName, hInstance);

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) DestroyWindow(hwnd);
			return 0;

		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}