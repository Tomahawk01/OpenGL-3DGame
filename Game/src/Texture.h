#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"

#include <cstddef>
#include <cstdint>
#include <span>

namespace Game {

	class Texture
	{
	public:
		Texture(std::span<const std::byte> data, std::uint32_t width, std::uint32_t height);

		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
	};

}
