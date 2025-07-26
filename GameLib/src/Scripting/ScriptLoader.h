#pragma once

#include "TLV/TLVReader.h"

#include <string>
#include <optional>

namespace Game {

	class ScriptLoader
	{
	public:
		ScriptLoader(const std::string& name);
		ScriptLoader(const std::string& name, const TLVReader& reader);

		std::string Load() const;

		std::string GetName() const;

	private:
		std::string m_Name;
		std::optional<const TLVReader*> m_Reader;
	};

}
