#pragma once

#include "MappedFile.h"

#include <cstddef>
#include <filesystem>
#include <string_view>
#include <vector>

namespace Game {

	class ResourceLoader
	{
	public:
		ResourceLoader(const std::filesystem::path& root);

		MappedFile Load(std::string_view name) const;

	private:
		std::filesystem::path m_Root;
	};

}
