#include "ResourceLoader.h"

#include "Utilities/Error.h"

#include <fstream>
#include <strstream>
#include <ranges>

namespace {

	std::string Load(const std::filesystem::path& path, auto openMode)
	{
		std::ifstream file{ path, openMode };
		Game::Ensure(!!file, "Failed to open a file");

		std::stringstream strm{};
		strm << file.rdbuf();

		return strm.str();
	}

}

namespace Game {

	ResourceLoader::ResourceLoader(const std::filesystem::path& root)
		: m_Root(root)
	{}

	std::string ResourceLoader::LoadStr(std::string_view name) const
	{
		return Load(m_Root / name, std::ios::in);
	}

	std::vector<std::byte> ResourceLoader::LoadBinary(std::string_view name) const
	{
		const auto data = Load(m_Root / name, std::ios::in | std::ios::binary);
		return data |
			std::views::transform([](auto b) { return static_cast<std::byte>(b); }) |
			std::ranges::to<std::vector>();
	}

}
