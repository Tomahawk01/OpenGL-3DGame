#include "ResourceLoader.h"

#include "Utilities/Error.h"

#include <fstream>
#include <strstream>
#include <ranges>

namespace Game {

	ResourceLoader::ResourceLoader(const std::filesystem::path& root)
		: m_Root(root)
	{}

	MappedFile ResourceLoader::Load(std::string_view name) const
	{
		return { m_Root / name };
	}

}
