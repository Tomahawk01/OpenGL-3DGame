#pragma once

#include "Utilities/Error.h"
#include "TLVReader.h"

namespace Game {

	inline TextFile GetFile(const TLVReader& reader, std::string_view name)
	{
		const auto basicVertFileEntry = std::ranges::find_if(reader, [name](const auto& e) { return e.IsTextFile(name); });
		Ensure(basicVertFileEntry != std::ranges::end(reader), "Could not find shader {}", name);
		return (*basicVertFileEntry).textFileValue();
	}

}
