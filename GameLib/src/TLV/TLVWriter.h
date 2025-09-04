#pragma once

#include "Renderer/Texture.h"
#include "Renderer/VertexData.h"
#include "Renderer/MeshData.h"
#include "Sound/SoundData.h"

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

		void Write(uint32_t value);
		void Write(std::span<const uint32_t> value);
		void Write(std::string_view value);
		void Write(std::span<const std::byte> value);
		void Write(TextureFormat value);
		void Write(TextureUsage value);
		void Write(const VertexData& value);
		void Write(std::span<const VertexData> value);
		void Write(
			std::string_view name,
			uint32_t width,
			uint32_t height,
			TextureFormat format,
			TextureUsage usage,
			std::span<const std::byte> data);
		void Write(std::string_view name, std::span<const VertexData> vertices, std::span<const uint32_t> indices);
		void Write(std::string_view name, std::string_view data);
		void Write(std::string_view name, std::span<const std::byte> format, std::span<const std::byte> data);

	private:
		std::vector<std::byte> m_Buffer;
	};

}
