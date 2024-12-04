#pragma once

#include "Exception.h"

namespace Game {

	template<class ...Args>
	void Ensure(bool predicate, std::string_view msg, Args&&... args)
	{
		if (!predicate)
			throw Exception(std::vformat(msg, std::make_format_args(std::forward(args)...)), 2u);
	}

}
