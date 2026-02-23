#pragma once
#include "pch.hpp"
#include "Dx12ApplicationLoader.hpp"

#include "WindowsApplication.hpp"
#include <cmath>

namespace Dx12Framework {

	class Dx12ApplicationLauncher : public WindowsApplication {
	public:
		Dx12DeviceContext context;
		Dx12RenderWindow renderWindow;
		Dx12ApplicationLoader loader;
		bool ShouldFlushFrame = false;



		uint32_t appIndex = 0U;
		bool running = true;

		static inline const wchar_t* appDlls[] = {
			TEXT("../applications/1_Dx12Triangle.dll"),
			TEXT("../applications/2_Dx12ConstantBuffer.dll"),
			TEXT("../applications/3_Dx12TexturesAndMips.dll")
		};

		Dx12ApplicationLauncher() : WindowsApplication(TEXT("Dx12Framework")),
			renderWindow(context.dxDirectQueue, context.pDxgiFactory.Get(), windowHandle),
			loader(context)
		{
			loader.Load(appDlls[appIndex]);
			++appIndex %= _countof(appDlls);
		}

		// Inherited via WindowsApplication
		bool Epoch(float timeDelta) override {
			if (ShouldFlushFrame) {
				SetWindowTextW(windowHandle, loader->GetApplicationName());
				renderWindow.FlushFrame();
				ShouldFlushFrame = false;
			}

			loader->Update(timeDelta);

			Dx12Framework::Dx12Frame& frame = renderWindow.NextFrame();
			frame.Begin();

			loader->Render(frame);

			frame.End();

			return running;
		}

		void OnKeyPress(Dx12Framework::KeyInfo kInfo) override {

			if (kInfo == VK_UP && kInfo.newPress) {
				loader.Load(appDlls[appIndex]);
				++appIndex %= (_countof(appDlls));
				ShouldFlushFrame = true;
			}

			if (kInfo == VK_DOWN && kInfo.newPress) {
				loader.Load(appDlls[appIndex]);
				--appIndex %= (_countof(appDlls));
				ShouldFlushFrame = true;
			}

			if (kInfo == VK_ESCAPE) {
				running = false;
			}
		}

		void OnKeyRelease(Dx12Framework::KeyInfo kInfo) override {

		}

	};
}