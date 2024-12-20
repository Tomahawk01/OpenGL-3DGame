#pragma once

#include "Math/VertexData.h"
#include "Utilities/StringMap.h"

#include <cstdint>
#include <span>
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
		ModelData Cube();

	private:
		struct LoadedModelData
		{
			std::vector<VertexData> vertices;
			std::vector<std::uint32_t> indices;
		};

		StringMap<LoadedModelData> m_LoadedModels;
	};

}
