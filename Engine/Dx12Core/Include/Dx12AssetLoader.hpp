#pragma once
#include "Dx12.hpp"

#include <filesystem>
#include <DirectXMath.h>
#include <vector>

namespace Dx12Framework {

	struct Dx12StandardVertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 normal;
	};

	class Dx12AssetLoader {
	public:
		static void LoadModel(std::filesystem::path path, std::vector<Dx12Framework::Dx12StandardVertex>& v, std::vector<uint32_t>& i);

	};
}
