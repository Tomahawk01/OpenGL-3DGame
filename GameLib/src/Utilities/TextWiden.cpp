#include "TextWiden.h"

#include "Error.h"

#include <Windows.h>

namespace Game {

	std::wstring TextWiden(std::string_view str)
	{
		const auto numWideChars = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, str.data(), static_cast<int>(str.size()), nullptr, 0);
		Ensure(numWideChars != 0, "Failed to get wstring size for converting: {}", str);

		auto wideStr = std::wstring(numWideChars, L'\0');

		Ensure(MultiByteToWideChar(CP_UTF8,
								   MB_PRECOMPOSED,
								   str.data(),
								   static_cast<int>(str.size()),
								   wideStr.data(),
								   static_cast<int>(wideStr.size())) == static_cast<int>(wideStr.size()), "Failed to widen string: {}", str);

		return wideStr;
	}

	std::string TextNarrow(std::wstring_view str)
	{
		const auto numMultiChars = WideCharToMultiByte(CP_UTF8, WC_NO_BEST_FIT_CHARS, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
		Ensure(numMultiChars != 0, "Failed to get string size for converting");

		auto narrowStr = std::string(numMultiChars, '\0');

		Ensure(WideCharToMultiByte(CP_UTF8,
								   WC_NO_BEST_FIT_CHARS,
								   str.data(),
								   static_cast<int>(str.size()),
								   narrowStr.data(),
								   static_cast<int>(narrowStr.size()),
								   nullptr,
								   nullptr) == static_cast<int>(narrowStr.size()), "Failed to narrow string");

		return narrowStr;
	}

}
