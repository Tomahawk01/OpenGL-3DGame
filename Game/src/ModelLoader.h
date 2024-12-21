#pragma once

#include "Math/VertexData.h"
#include "Utilities/StringMap.h"
#include "ResourceLoader.h"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace Game {

	struct ModelData
	{
		std::span<const VertexData> vertices;
		std::span<const std::uint32_t> indices;
	};

	class ModelLoader
	{
	public:
		ModelLoader(ResourceLoader& resourceLoader);

		ModelData Cube();

		ModelData Load(std::string_view modelFile, std::string_view modelName);

	private:
		struct LoadedModelData
		{
			std::vector<VertexData> vertices;
			std::vector<std::uint32_t> indices;
		};

		StringMap<LoadedModelData> m_LoadedModels;
		ResourceLoader& m_ResourceLoader;
	};

}
