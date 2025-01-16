#include "CubeMap.h"

#include "Utilities/Error.h"
#include "TLVReader.h"

#include <stb_image.h>

#include <tuple>
#include <algorithm>

namespace Game {

	CubeMap::CubeMap(const std::vector<std::span<const std::byte>>& faces, std::uint32_t width, std::uint32_t height)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
	{
		::glCreateTextures(GL_TEXTURE_CUBE_MAP, 1u, &m_Handle);
		::glTextureStorage2D(m_Handle, 1, GL_SRGB8, width, height);

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

	CubeMap::CubeMap(const TLVReader& reader, std::array<std::string_view, 6> imageNames)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
	{
		const auto descs = imageNames | std::views::transform([&reader](const auto& e) {
			const auto desc = std::ranges::find_if(reader, [e](const auto& e2) { return e2.IsTexture(e); });
			Ensure(desc != std::ranges::end(reader), "Cannot find image");
			return (*desc).textureDescriptionValue();
		});

		const std::uint32_t width = descs.front().width;
		const std::uint32_t height = descs.front().height;

		Ensure(std::ranges::all_of(descs |
			std::views::transform([](const auto& e)
			{
				return std::make_tuple(e.width, e.height);
			}) | std::views::drop(1),
			[width, height](const auto& e)
			{
				return e == std::make_tuple(width, height);
			}), "All widths and heights need to be the same");

		::glCreateTextures(GL_TEXTURE_CUBE_MAP, 1u, &m_Handle);
		::glTextureStorage2D(m_Handle, 1, GL_SRGB8, width, height);

		for (const auto& [index, desc] : std::views::enumerate(descs))
		{
			::glTextureSubImage3D(m_Handle, 0, 0, 0, static_cast<::GLint>(index), desc.width, desc.height, 1, GL_RGB, GL_UNSIGNED_BYTE, desc.data.data());
		}
	}

	::GLuint CubeMap::GetNativeHandle() const
	{
		return m_Handle;
	}

}
