#pragma once

#include "Renderer/VertexData.h"

#include <cstdint>
#include <span>

namespace Game {

	struct MeshData
	{
		std::span<const VertexData> vertices;
		std::span<const uint32_t> indices;
	};

}
