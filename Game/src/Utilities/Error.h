#pragma once

#include "AutoRelease.h"
#include "Exception.h"

namespace Game {

	template<class ...Args>
	void Ensure(bool predicate, std::string_view msg, Args&&... args)
	{
		if (!predicate)
			throw Exception(std::vformat(msg, std::make_format_args(std::forward<Args>(args)...)), 2u);
	}

	template<class T, T Invalid, class ...Args>
	void Ensure(AutoRelease<T, Invalid>& obj, std::string_view msg, Args&&... args)
	{
		Ensure(!!obj, msg, std::forward<Args>(args)...);
	}

}
