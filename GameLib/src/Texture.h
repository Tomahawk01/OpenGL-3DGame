#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"

#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <string_view>

namespace Game {

	class TLVReader;

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
		std::uint32_t width;
		std::uint32_t height;
		TextureFormat format;
		TextureUsage usage;
		std::vector<std::byte> data;
	};

	class Texture
	{
	public:
		Texture(TextureUsage usage, std::span<const std::byte> data, std::uint32_t width, std::uint32_t height);
		Texture(const TextureDescription& description);
		Texture(const TLVReader& reader, std::string_view name);
		Texture(TextureUsage usage, std::uint32_t width, std::uint32_t height);

		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
	};

}

template<>
struct std::formatter<Game::TextureDescription>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::TextureDescription& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "width={} height={} format={} usage={} data={}",
			obj.width,
			obj.height,
			std::to_underlying(obj.format),
			std::to_underlying(obj.usage),
			obj.data.size());
	}
};
