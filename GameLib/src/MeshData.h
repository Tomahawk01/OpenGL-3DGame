#pragma once

#include "Math/VertexData.h"

#include <cstdint>
#include <span>

namespace Game {

	struct MeshData
	{
		std::span<const VertexData> vertices;
		std::span<const std::uint32_t> indices;
	};

}
