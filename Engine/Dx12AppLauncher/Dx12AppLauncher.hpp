#include <imgui.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>

#include <WindowsApplication.hpp>
#include <Dx12DeviceContext.hpp>
#include <Dx12RenderWindow.hpp>
#include <Dx12Application.hpp>

#include <DirectXColors.h>
using namespace DirectX;
using namespace Dx12Framework;

#include <filesystem>
#include <fstream> // added
#include <string>
// Add at top of file near other includes
#include <optional>

namespace fs = std::filesystem;

// Global to remember previous DX12_PLUGIN_ASSET_ROOT value so we can restore it on exit/failure.
static std::string g_prevAssetEnv;
static bool g_prevAssetEnvPresent = false;

namespace Dx12Framework {

	// From ImGui DirectX12 Example Code
	struct ExampleDescriptorHeapAllocator
	{
		ID3D12DescriptorHeap* Heap = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
		D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
		D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
		UINT                        HeapHandleIncrement;
		ImVector<int>               FreeIndices;

		void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
		{
			IM_ASSERT(Heap == nullptr && FreeIndices.empty());
			Heap = heap;
			D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
			HeapType = desc.Type;
			HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
			HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
			HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
			FreeIndices.reserve((int)desc.NumDescriptors);
			for (int n = desc.NumDescriptors; n > 0; n--)
				FreeIndices.push_back(n - 1);
		}
		void Destroy()
		{
			Heap = nullptr;
			FreeIndices.clear();
		}
		void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
		{
			IM_ASSERT(FreeIndices.Size > 0);
			int idx = FreeIndices.back();	// Exception thrown: read access violation. ImVector<int>::back(...) returned 0x16C0DE9BE8C.
			FreeIndices.pop_back();
			out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
			out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
		}
		void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
		{
			int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
			int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
			IM_ASSERT(cpu_idx == gpu_idx);
			FreeIndices.push_back(cpu_idx);
		}
	};



	class Dx12ApplicationLauncher : public WindowsApplication {
	public:
		Dx12DeviceContext ctx;
		Dx12RenderWindow rw;
		HMODULE dll = nullptr;
		Dx12Application* pApp = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pImGuiSrvDescriptorHeap;
		ExampleDescriptorHeapAllocator srvAlloc;
		static constexpr uint32_t APP_SRV_HEAP_SIZE = 64;

		std::vector<std::string> appDlls;

		bool skipFrame = false;

		Dx12ApplicationLauncher();

		~Dx12ApplicationLauncher() override;

		void LoadDll(const fs::path& path);


		void RefreshAvailablePlugins();

		std::optional<std::string> nextDll;

		void ShowLauncherUI();

		// Inherited via WindowsApplication
		bool Epoch(float timeDelta) override;
		

		void OnKeyPress(KeyInfo key) override {
		}

		void OnKeyRelease(KeyInfo key) override {
		}

	};
}


/*

{
			if (skipFrame) {
				ctx.dxDirectQueue.Flush();
				ctx.dxCopyQueue.Flush();
				rw.FlushFrame();
				skipFrame = false;

				if (nextDll.has_value()) {
					std::string dllPath = nextDll.value();

					LoadDll(nextDll.value());
					nextDll.reset();
				}
			}

			Dx12Frame& f = rw.NextFrame();

			if (pApp) pApp->Update(timeDelta);

			f.Begin();

			if (!pApp) {
				constexpr FLOAT clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
				f.Clear(clearColor);
			}

			// Start the Dear ImGui frame
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (pApp) pApp->Render(f);

			ShowLauncherUI();

			// Rendering
			ImGui::Render();


			ID3D12DescriptorHeap* heaps[] = { pImGuiSrvDescriptorHeap.Get() };
			f->SetDescriptorHeaps(1, heaps);
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), f.GetCommandList());

			f.End();

			return true;
		}
*/