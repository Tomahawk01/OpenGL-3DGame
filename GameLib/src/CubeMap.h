#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <span>

namespace Game {

	class CubeMap
	{
	public:
		CubeMap(const std::vector<std::span<const std::byte>>& faces, std::uint32_t width, std::uint32_t height);

		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
	};

}
