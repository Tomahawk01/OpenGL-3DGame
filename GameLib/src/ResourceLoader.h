#pragma once

#include "File.h"

#include <cstddef>
#include <filesystem>
#include <string_view>
#include <vector>

namespace Game {

	class ResourceLoader
	{
	public:
		ResourceLoader(const std::filesystem::path& root);

		File Load(std::string_view name) const;

	private:
		std::filesystem::path m_Root;
	};

}
