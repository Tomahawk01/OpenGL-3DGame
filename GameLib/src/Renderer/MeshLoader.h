#pragma once

#include "Renderer/VertexData.h"
#include "Utilities/StringMap.h"
#include "Renderer/MeshData.h"
#include "Core/ResourceLoader.h"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace Game {

	class MeshLoader
	{
	public:
		MeshLoader(ResourceLoader& resourceLoader);

		MeshData Cube();
		MeshData Sprite();

		MeshData Load(std::string_view meshFile, std::string_view meshName);

	private:
		struct LoadedMeshData
		{
			std::vector<VertexData> vertices;
			std::vector<uint32_t> indices;
		};

		StringMap<LoadedMeshData> m_LoadedMeshes;
		ResourceLoader& m_ResourceLoader;
	};

}
