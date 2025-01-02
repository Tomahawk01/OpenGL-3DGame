#pragma once

#include "Math/VertexData.h"
#include "Utilities/StringMap.h"
#include "ResourceLoader.h"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace Game {

	struct MeshData
	{
		std::span<const VertexData> vertices;
		std::span<const std::uint32_t> indices;
	};

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
			std::vector<std::uint32_t> indices;
		};

		StringMap<LoadedMeshData> m_LoadedMeshes;
		ResourceLoader& m_ResourceLoader;
	};

}
