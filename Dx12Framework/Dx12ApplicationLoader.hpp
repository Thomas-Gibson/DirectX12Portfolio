#pragma once
#include "pch.hpp"
#include "Dx12Application.hpp"

namespace Dx12Framework {

	class Dx12ApplicationLoader {
	public:
		HMODULE dll = nullptr;
		std::unique_ptr<Dx12Framework::Dx12Application> app = nullptr;
		Dx12Framework::Dx12DeviceContext& dx12Context;

		Dx12ApplicationLoader(Dx12DeviceContext& dx12Context) : dx12Context(dx12Context) {}
		~Dx12ApplicationLoader() { Unload(); }

		void Load(const wchar_t* path) {
			Unload();
			HMODULE appDll = LoadLibrary(path);
			
			if (!appDll) {
				throw std::system_error(GetLastError(), std::system_category());
			}

			Dx12Framework::Dx12Application* (*GetApplication)(Dx12Framework::Dx12DeviceContext&);
			GetApplication = reinterpret_cast<decltype(GetApplication)>(GetProcAddress(appDll, "GetApplication"));

			if (!GetApplication) {
				FreeLibrary(appDll);
				throw std::system_error(GetLastError(), std::system_category());
			}

			dll = appDll;
			app = std::unique_ptr< Dx12Framework::Dx12Application>(GetApplication(dx12Context));
		}

		void Unload() {
			if (app) {
				dx12Context.dxDirectQueue.Flush();
				app.reset();
			}

			if (dll) {
				FreeLibrary(dll);
				dll = nullptr;
			}
		}

		Dx12Framework::Dx12Application* operator ->() const { return app.get(); }
	};

}