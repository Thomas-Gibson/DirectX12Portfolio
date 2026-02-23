#pragma once
/*
* Author: Thomas Gibson
* Created: Feb 14, 2026
*
*/

#include "Dx12CommandQueue.hpp"
#include "Dx12RenderWindow.hpp"
#include "Dx12DeviceContext.hpp"
namespace Dx12Framework {

	class Dx12Application {
	public:
		virtual ~Dx12Application() = default;

		virtual void Update(float timeDelta) = 0;
		virtual void Render(const Dx12Framework::Dx12Frame& frame) = 0;

		virtual const wchar_t* GetApplicationName() const { return L"Dx12Framework"; };
	};
}