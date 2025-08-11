#pragma once

#include "Math/Color.h"

#include <string_view>

namespace Game {

	class Texture;
	class Sampler;

	class TextFactory
	{
	public:
		Texture Create(std::string_view text, const Sampler* sampler, uint32_t fontSize = 24u, const Color& color = Colors::White) const;
	};

}
