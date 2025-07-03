#include "File.h"

#include "Utilities/Error.h"

#include <tuple>

namespace {

	auto Init(const std::filesystem::path& path, size_t size)
	{
		Game::AutoRelease<HANDLE, INVALID_HANDLE_VALUE> handle{
			::CreateFileA(path.string().c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, size != 0u ? CREATE_NEW : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr),
			::CloseHandle
		};
		Game::Ensure(handle, "Failed to open file");

		Game::AutoRelease<HANDLE, reinterpret_cast<HANDLE>(NULL)> mapping{
			::CreateFileMappingA(handle, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(size), nullptr),
			::CloseHandle
		};
		Game::Ensure(mapping, "Failed to map file: error code {}", ::GetLastError());

		std::unique_ptr<void, decltype(&::UnmapViewOfFile)> mapView{
			::MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0),
			::UnmapViewOfFile
		};
		Game::Ensure(mapView, "Failed to get map view");

		return std::make_tuple(std::move(handle), std::move(mapping), std::move(mapView));
	}

}

namespace Game {

	File::File(const std::filesystem::path& path)
		: m_Handle{ INVALID_HANDLE_VALUE, ::CloseHandle }
		, m_Mapping{ NULL, ::CloseHandle }
		, m_MapView{ nullptr, ::UnmapViewOfFile }
		, m_Size{}
	{
		auto [handle, mapping, mapView] = Init(path, 0u);
		m_Handle = std::move(handle);
		m_Mapping = std::move(mapping);
		m_MapView = std::move(mapView);
		m_Size = ::GetFileSize(m_Handle, nullptr);
	}

	File::File(const std::filesystem::path& path, size_t size)
		: m_Handle{ INVALID_HANDLE_VALUE, ::CloseHandle }
		, m_Mapping{ NULL, ::CloseHandle }
		, m_MapView{ nullptr, ::UnmapViewOfFile }
		, m_Size{ size }
	{
		auto [handle, mapping, mapView] = Init(path, size);
		m_Handle = std::move(handle);
		m_Mapping = std::move(mapping);
		m_MapView = std::move(mapView);
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
