#pragma once

#include <string_view>

namespace Game {

	class Texture;

	class TextFactory
	{
	public:
		Texture* Create(std::string_view text) const;

	private:
	};

}
