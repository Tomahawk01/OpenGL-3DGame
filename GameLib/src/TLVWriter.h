#pragma once

#include "Texture.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace Game {

	class TLVWriter
	{
	public:
		std::vector<std::byte> yield();

		void Write(std::uint32_t value);
		void Write(std::string_view value);
		void Write(std::span<const std::byte> value);
		void Write(TextureFormat value);
		void Write(TextureUsage value);
		void Write(
			std::string_view name,
			std::uint32_t width,
			std::uint32_t height,
			TextureFormat format,
			TextureUsage usage,
			std::span<const std::byte> data);

	private:
		std::vector<std::byte> m_Buffer;
	};

}
