#include "File.h"

#include "Utilities/Error.h"

namespace Game {

	File::File(const std::filesystem::path& path)
		: m_Handle{ INVALID_HANDLE_VALUE, ::CloseHandle }
		, m_Mapping{ NULL, ::CloseHandle }
		, m_MapView{ nullptr, ::UnmapViewOfFile }
		, m_Size{}
	{
		m_Handle.Reset(::CreateFileA(path.string().c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
		Ensure(m_Handle, "Failed to open file");

		m_Mapping.Reset(::CreateFileMappingA(m_Handle, nullptr, PAGE_READWRITE, 0, 0, nullptr));
		Ensure(m_Mapping, "Failed to map file");

		m_MapView.reset(::MapViewOfFile(m_Mapping, FILE_MAP_READ, 0, 0, 0));
		Ensure(m_MapView, "Failed to get map view");

		m_Size = ::GetFileSize(m_Handle, nullptr);
	}

	std::string_view File::AsString() const
	{
		return { reinterpret_cast<const char*>(m_MapView.get()), m_Size };
	}

	std::span<const std::byte> File::AsData() const
	{
		return { reinterpret_cast<const std::byte*>(m_MapView.get()), m_Size };
	}

}
