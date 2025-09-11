#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace Game {

	class TLVReader;
	class Sampler;

	enum class TextureUsage
	{
		FRAMEBUFFER,
		DEPTH,
		SRGB,
		DATA
	};

	enum class TextureFormat
	{
		RGB,
		RGBA
	};

	struct TextureDescription
	{
		uint32_t width;
		uint32_t height;
		TextureFormat format;
		TextureUsage usage;
		std::vector<std::byte> data;
	};

	class Texture
	{
	public:
		Texture(TextureUsage usage, std::span<const std::byte> data, uint32_t width, uint32_t height, const Sampler* sampler);
		Texture(TextureUsage usage, std::span<const std::byte> data, uint32_t numChannels, uint32_t width, uint32_t height, const Sampler* sampler);
		Texture(const TextureDescription& description, const Sampler* sampler);
		Texture(const TLVReader& reader, std::string_view name, const Sampler* sampler);
		Texture(TextureUsage usage, uint32_t width, uint32_t height, uint8_t samples);

		::GLuint GetNativeHandle() const;
		const Sampler* GetSampler() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		AutoRelease<::GLuint> m_Handle;
		const Sampler* m_Sampler;
		uint32_t m_Width;
		uint32_t m_Height;
	};

	std::string to_string(const TextureDescription& obj);

}
