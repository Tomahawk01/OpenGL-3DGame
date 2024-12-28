#include "CubeMap.h"

#include "Utilities/Error.h"

#include <stb_image.h>

namespace Game {

	CubeMap::CubeMap(const std::vector<std::span<const std::byte>>& faces, std::uint32_t width, std::uint32_t height)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
	{
		::glCreateTextures(GL_TEXTURE_CUBE_MAP, 1u, &m_Handle);
		::glTextureStorage2D(m_Handle, 1, GL_RGBA8, width, height);

		for (const auto& [index, face] : std::views::enumerate(faces))
		{
			int w{};
			int h{};
			int numChannels{};

			auto rawData = std::unique_ptr<::stbi_uc, void(*)(void*)>(
				::stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(face.data()), static_cast<int>(face.size()), &w, &h, &numChannels, 0),
				::stbi_image_free
			);
			Ensure(rawData, "Failed to parse texture data");
			Ensure(static_cast<std::uint32_t>(w) == width, "Width has changed");
			Ensure(static_cast<std::uint32_t>(h) == height, "Height has changed");

			::glTextureSubImage3D(m_Handle, 0, 0, 0, static_cast<::GLint>(index), width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, rawData.get());
		}
	}

	::GLuint CubeMap::GetNativeHandle() const
	{
		return m_Handle;
	}

}
