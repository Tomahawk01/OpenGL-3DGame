#pragma once

#include "Utilities/AutoRelease.h"

#include <Windows.h>

#include <cstddef>
#include <memory>
#include <filesystem>
#include <string_view>

namespace Game {

	enum class CreationMode
	{
		OPEN,
		CREATE
	};

	class File
	{
	public:
		File(const std::filesystem::path& path, CreationMode mode = CreationMode::OPEN);

		template <class T>
		void Write(const T& data) requires(sizeof(std::ranges::range_value_t<T>) == 1)
		{
			std::memcpy(m_MapView.get(), std::ranges::data(data), std::ranges::size(data));
			Ensure(::FlushViewOfFile(m_MapView.get(), data.size()) != 0, "Failed to flush file");
		}

		std::string_view AsString() const;
		std::span<const std::byte> AsData() const;

	private:
		AutoRelease<HANDLE, INVALID_HANDLE_VALUE> m_Handle;
		AutoRelease<HANDLE, reinterpret_cast<HANDLE>(NULL)> m_Mapping;
		std::unique_ptr<void, decltype(&::UnmapViewOfFile)> m_MapView;
		size_t m_Size;
	};

}
