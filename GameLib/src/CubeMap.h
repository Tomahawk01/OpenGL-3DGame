#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <span>
#include <string_view>

namespace Game {

	class TLVReader;

	class CubeMap
	{
	public:
		CubeMap(const std::vector<std::span<const std::byte>>& faces, std::uint32_t width, std::uint32_t height);
		CubeMap(const TLVReader& reader, std::array<std::string_view, 6> imageNames);

		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
	};

}
