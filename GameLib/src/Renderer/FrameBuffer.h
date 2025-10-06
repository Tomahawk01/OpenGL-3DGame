#pragma once

#include "Utilities/AutoRelease.h"
#include "Texture.h"
#include "OpenGL.h"

#include <cstdint>
#include <span>

namespace Game {

	class FrameBuffer
	{
	public:
		FrameBuffer(std::span<const Texture*> colorTextures, const Texture* depthTexture);

		void Bind() const;
		void UnBind() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		::GLuint GetNativeHandle() const;
		std::span<const Texture*> GetColorTextures() const;

	private:
		AutoRelease<::GLuint> m_Handle;
		std::span<const Texture*> m_ColorTextures;
		const Texture* m_DepthTexture;
	};

}
