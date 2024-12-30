#include "Texture.h"

#include "Utilities/Error.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>

namespace Game {

	Texture::Texture(TextureUsage usage, std::span<const std::byte> data, std::uint32_t width, std::uint32_t height)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
	{
		TextureUsage validUsage[] = { TextureUsage::SRGB, TextureUsage::DATA };
		Ensure(std::ranges::contains(validUsage, usage), "Invalid usage");

		int w{};
		int h{};
		int numChannels{};

		auto rawData = std::unique_ptr<::stbi_uc, void(*)(void*)>(
			::stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()), static_cast<int>(data.size()), &w, &h, &numChannels, 0),
			::stbi_image_free
		);
		Ensure(rawData, "Failed to parse texture data");
		Ensure(static_cast<std::uint32_t>(w) == width, "Width has changed");
		Ensure(static_cast<std::uint32_t>(h) == height, "Height has changed");

		::glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

		::GLenum format{};
		switch (usage)
		{
		case TextureUsage::SRGB:
			format = numChannels == 4 ? GL_SRGB8_ALPHA8 : GL_SRGB8;
			break;
		case TextureUsage::DATA:
			format = numChannels == 4 ? GL_RGBA8 : GL_RGB8;
			break;
		default:
			break;
		}

		::glTextureStorage2D(m_Handle, 1, format, width, height);
		::glTextureSubImage2D(m_Handle, 0, 0, 0, width, height, numChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, rawData.get());
	}

	Texture::Texture(TextureUsage usage, std::uint32_t width, std::uint32_t height)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
	{
		::glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
		switch (usage)
		{
		case TextureUsage::FRAMEBUFFER:
			::glTextureStorage2D(m_Handle, 1, GL_RGB16F, width, height);
			break;
		case TextureUsage::DEPTH:
			::glTextureStorage2D(m_Handle, 1, GL_DEPTH_COMPONENT24, width, height);
			break;
		default:
			break;
		}
	}

	::GLuint Texture::GetNativeHandle() const
	{
		return m_Handle;
	}

}
