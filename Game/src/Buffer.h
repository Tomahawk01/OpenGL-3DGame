#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"

#include <span>
#include <cstdint>
#include <cstddef>

namespace Game {

	class Buffer
	{
	public:
		Buffer(std::uint32_t size);

		void Write(std::span<const std::byte> data, std::size_t offset) const;

		GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Buffer;
	};

}
