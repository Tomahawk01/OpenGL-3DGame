#include "Texture.h"

#include "Utilities/Error.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Game {

	Texture::Texture(std::span<const std::byte> data, std::uint32_t width, std::uint32_t height)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
	{
		int w = static_cast<int>(width);
		int h = static_cast<int>(height);
		int numChannels{ 4 };

		auto rawData = std::unique_ptr<::stbi_uc, void(*)(void*)>(
			::stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data.data()), static_cast<int>(data.size()), &w, &h, &numChannels, 0),
			::stbi_image_free
		);
		Ensure(rawData, "Failed to parse texture data");
		Ensure(static_cast<std::uint32_t>(w) == width, "Width has changed");
		Ensure(static_cast<std::uint32_t>(h) == height, "Height has changed");
		Ensure(numChannels == 4, "Incorrect number of channels: {}", numChannels);

		::glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

		::glTextureStorage2D(m_Handle, 1, GL_RGBA8, width, height);
		::glTextureSubImage2D(m_Handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rawData.get());
	}

	::GLuint Texture::GetNativeHandle() const
	{
		return m_Handle;
	}

}
