#include "Dx12AssetLoader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

void Dx12Framework::Dx12AssetLoader::LoadModel(std::filesystem::path path, std::vector<Dx12StandardVertex>& vertices, std::vector<uint32_t>& indicies) {
	Assimp::Importer importer;

	const uint32_t flags = aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_GenUVCoords;
	const aiScene* pScene = importer.ReadFile(path.string().c_str(), flags);

	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
		return;
	}

	vertices.clear();
	indicies.clear();
	vertices.reserve(pScene->mMeshes[0]->mNumVertices);
	indicies.reserve(pScene->mMeshes[0]->mNumFaces * 3);

	uint32_t indexOffset = 0;

	for (uint32_t m = 0; m < pScene->mNumMeshes; m++) {

		aiMesh* pMesh = pScene->mMeshes[m];

		for (uint32_t i = 0; i < pMesh->mNumVertices; i++) {
			Dx12StandardVertex v = {};

			if (pMesh->HasPositions()) {
				aiVector3D pos = pMesh->mVertices[i];
				v.position = { pos.x, pos.y, pos.z };
			}

			if (pMesh->HasTangentsAndBitangents()) {
				aiVector3D tan = pMesh->mTangents[i];
				v.tangent = { tan.x, tan.y, tan.z };
			}

			if (pMesh->HasNormals()) {
				aiVector3D norm = pMesh->mNormals[i];
				v.normal = { norm.x, norm.y, norm.z };
			}

			if (pMesh->HasTextureCoords(i)) {
				aiVector3D* uv = pMesh->mTextureCoords[i];
				v.texCoord = { uv->x, uv->y };
			}

			vertices.push_back(v);
		}

		for (uint32_t i = 0; i < pMesh->mNumFaces; i++) {
			aiFace face = pMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indicies.push_back(face.mIndices[j] + indexOffset);
			}
		}

		indexOffset += pMesh->mNumVertices;
	}
}