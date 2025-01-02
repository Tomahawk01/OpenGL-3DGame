#pragma once

#include <cstddef>
#include <filesystem>
#include <string_view>
#include <vector>

namespace Game {

	class ResourceLoader
	{
	public:
		ResourceLoader(const std::filesystem::path& root);

		std::string LoadStr(std::string_view name) const;
		std::vector<std::byte> LoadBinary(std::string_view name) const;

	private:
		std::filesystem::path m_Root;
	};

}
