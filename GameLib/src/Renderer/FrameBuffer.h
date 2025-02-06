#pragma once

#include "Utilities/AutoRelease.h"
#include "Texture.h"
#include "OpenGL.h"

#include <cstdint>

namespace Game {

	class FrameBuffer
	{
	public:
		FrameBuffer(uint32_t width, uint32_t height);

		void Bind() const;
		void UnBind() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		::GLuint GetNativeHandle() const;
		const Texture& GetColorTexture() const;

	private:
		AutoRelease<::GLuint> m_Handle;
		uint32_t m_Width;
		uint32_t m_Height;
		Texture m_ColorTexture;
		Texture m_DepthTexture;
	};

}
