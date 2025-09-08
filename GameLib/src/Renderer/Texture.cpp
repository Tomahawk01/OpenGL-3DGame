#include "Texture.h"

#include "Utilities/Formatter.h"
#include "Utilities/Error.h"
#include "TLV/TLVReader.h"
#include "Sampler.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>

namespace {

	::GLenum toOpenGL(Game::TextureUsage usage, Game::TextureFormat format)
	{
		switch (usage)
		{
		case Game::TextureUsage::SRGB:
			return format == Game::TextureFormat::RGBA ? GL_SRGB8_ALPHA8 : GL_SRGB8;
		case Game::TextureUsage::DATA:
			return format == Game::TextureFormat::RGBA ? GL_RGBA8 : GL_RGB8;
		default:
			throw Game::Exception("Unknown usage");
		}
	}

}

namespace Game {

	Texture::Texture(TextureUsage usage, std::span<const std::byte> data, uint32_t width, uint32_t height, const Sampler* sampler)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
		, m_Sampler(sampler)
		, m_Width{ width }
		, m_Height{ height }
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
		Ensure(static_cast<uint32_t>(w) == width, "Width has changed");
		Ensure(static_cast<uint32_t>(h) == height, "Height has changed");

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

		::glTextureStorage2D(m_Handle, 1 + static_cast<::GLint>(std::floor(std::log(std::max(m_Width, m_Height)))), format, width, height);
		::glTextureSubImage2D(m_Handle, 0, 0, 0, width, height, numChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, rawData.get());
		::glGenerateTextureMipmap(m_Handle);
	}

	Texture::Texture(TextureUsage usage, std::span<const std::byte> data, uint32_t numChannels, uint32_t width, uint32_t height, const Sampler* sampler)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
		, m_Sampler(sampler)
		, m_Width{ width }
		, m_Height{ height }
	{
		TextureUsage validUsage[] = { TextureUsage::SRGB, TextureUsage::DATA };
		Ensure(std::ranges::contains(validUsage, usage), "Invalid usage");

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

		::glTextureStorage2D(m_Handle, 1 + static_cast<::GLint>(std::floor(std::log(std::max(m_Width, m_Height)))), format, width, height);
		::glTextureSubImage2D(m_Handle, 0, 0, 0, width, height, numChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data.data());
		::glGenerateTextureMipmap(m_Handle);
	}

	Texture::Texture(const TextureDescription& description, const Sampler* sampler)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
		, m_Sampler(sampler)
		, m_Width{ description.width }
		, m_Height{ description.height }
	{
		::glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

		::glTextureStorage2D(m_Handle, 1 + static_cast<::GLint>(std::floor(std::log(std::max(m_Width, m_Height)))), toOpenGL(description.usage, description.format), description.width, description.height);
		::glTextureSubImage2D(m_Handle, 0, 0, 0, description.width, description.height, description.format == TextureFormat::RGBA ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, description.data.data());
		::glGenerateTextureMipmap(m_Handle);
	}

	Texture::Texture(const TLVReader& reader, std::string_view name, const Sampler* sampler)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
		, m_Sampler(sampler)
	{
		auto desc = std::ranges::find_if(reader, [name](const auto& e) { return e.IsTexture(name); });
		Ensure(desc != std::ranges::end(reader), "Could not find texture");

		Texture tex{ (*desc).textureDescriptionValue(), m_Sampler };

		std::ranges::swap(m_Handle, tex.m_Handle);
	}

	Texture::Texture(TextureUsage usage, uint32_t width, uint32_t height)
		: m_Handle{ 0u, [](auto texture) { ::glDeleteTextures(1u, &texture); } }
		, m_Width{ width }
		, m_Height{ height }
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

	const Sampler* Texture::GetSampler() const
	{
		return m_Sampler;
	}

	uint32_t Texture::GetWidth() const
	{
		return m_Width;
	}

	uint32_t Texture::GetHeight() const
	{
		return m_Height;
	}

	std::string to_string(const TextureDescription& obj)
	{
		return std::format("width={} height={} format={} usage={} data={}",
			obj.width,
			obj.height,
			std::to_underlying(obj.format),
			std::to_underlying(obj.usage),
			obj.data.size());
	}

}
