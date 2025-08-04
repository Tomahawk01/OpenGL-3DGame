#pragma once

#include <string_view>

namespace Game {

	class Texture;
	class Sampler;

	class TextFactory
	{
	public:
		Texture Create(std::string_view text, const Sampler* sampler) const;

	private:
	};

}
