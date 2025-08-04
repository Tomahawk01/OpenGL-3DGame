#pragma once

#include <cstdint>

namespace Game {

	class Texture;

	struct Label
	{
		const Texture* texture;
		std::uint32_t x;
		std::uint32_t y;
	};

}
