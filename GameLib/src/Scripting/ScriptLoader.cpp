#include "ScriptLoader.h"

#include "Core/File.h"
#include "TLV/Utilities.h"

namespace Game {

	ScriptLoader::ScriptLoader(const std::string& name)
		: m_Name{ name }
		, m_Reader{}
	{}

	ScriptLoader::ScriptLoader(const std::string& name, const TLVReader& reader)
		: m_Name{ name }
		, m_Reader{ std::addressof(reader) }
	{}

	std::string ScriptLoader::Load() const
	{
		if (m_Reader)
		{
			return GetFile(**m_Reader, m_Name).Data;
		}

		const File file{ m_Name };

		return std::string{ file.AsString() };
	}

	std::string ScriptLoader::GetName() const
	{
		return m_Name;
	}

}
