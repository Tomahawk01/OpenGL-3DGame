#pragma once

#include "Utilities/AutoRelease.h"
#include "Texture.h"
#include "OpenGL.h"

#include <cstdint>

namespace Game {

	class FrameBuffer
	{
	public:
		FrameBuffer(std::uint32_t width, std::uint32_t height);

		void Bind() const;
		void UnBind() const;

		std::uint32_t GetWidth() const;
		std::uint32_t GetHeight() const;

		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
		std::uint32_t m_Width;
		std::uint32_t m_Height;
		Texture m_ColorTexture;
		Texture m_DepthTexture;
	};

}
