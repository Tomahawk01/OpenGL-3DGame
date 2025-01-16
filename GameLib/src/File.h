#pragma once

#include "Utilities/AutoRelease.h"

#include <Windows.h>

#include <cstddef>
#include <memory>
#include <filesystem>
#include <string_view>

namespace Game {

	class File
	{
	public:
		File(const std::filesystem::path& path);

		std::string_view AsString() const;
		std::span<const std::byte> AsData() const;

	private:
		AutoRelease<HANDLE, INVALID_HANDLE_VALUE> m_Handle;
		AutoRelease<HANDLE, reinterpret_cast<HANDLE>(NULL)> m_Mapping;
		std::unique_ptr<void, decltype(&::UnmapViewOfFile)> m_MapView;
		std::size_t m_Size;
	};

}
