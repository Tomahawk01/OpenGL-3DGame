#include "TextWiden.h"

#include "Error.h"

#include <Windows.h>

namespace Game {

	std::wstring TextWiden(std::string_view str)
	{
		const auto numWideChars = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, str.data(), static_cast<int>(str.size()), nullptr, 0);
		Ensure(numWideChars != 0, "Failed to get wstring size for converting: {}", str);

		auto wideStr = std::wstring(numWideChars, L'\0');

		Ensure(MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, str.data(), static_cast<int>(str.size()), wideStr.data(), static_cast<int>(wideStr.size())) == static_cast<int>(wideStr.size()), "Failed to widen string: {}", str);

		return wideStr;
	}

}
