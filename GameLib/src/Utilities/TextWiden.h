#pragma once

#include <string_view>

namespace Game {

	std::wstring TextWiden(std::string_view str);
	std::string TextNarrow(std::wstring_view str);

}
