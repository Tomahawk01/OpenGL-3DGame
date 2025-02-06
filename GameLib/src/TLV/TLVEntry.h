#pragma once

#include "Renderer/Texture.h"
#include "Renderer/VertexData.h"
#include "Renderer/MeshData.h"

#include <cstdint>
#include <cstddef>
#include <format>
#include <string>
#include <span>
#include <vector>

namespace Game {

	enum class TLVType : uint32_t
	{
		UINT32,
		UINT32_ARRAY,
		STRING,
		BYTE_ARRAY,
		TEXTURE_FORMAT,
		TEXTURE_USAGE,
		VERTEX_DATA,
		VERTEX_DATA_ARRAY,

		TEXTURE_DESCRIPTION,
		MESH_DATA
	};

	class TLVEntry
	{
	public:
		TLVEntry(TLVType type, std::span<const std::byte> value);

		uint32_t uint32Value() const;
		std::vector<uint32_t> uint32ArrayValue() const;
		std::string stringValue() const;
		std::vector<std::byte> byteArrayValue() const;
		TextureFormat textureFormatValue() const;
		TextureUsage textureUsageValue() const;
		TextureDescription textureDescriptionValue() const;
		bool IsTexture(std::string_view name) const;
		VertexData vertexDataValue() const;
		std::vector<VertexData> vertexDataArrayValue() const;
		MeshData meshValue() const;
		bool IsMesh(std::string_view name) const;

		TLVType Type() const;
		uint32_t Size() const;

	private:
		TLVType m_Type;
		std::span<const std::byte> m_Value;
	};

}

template<>
struct std::formatter<Game::TLVType>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::TLVType& obj, std::format_context& ctx) const
	{
		auto str = "unknown"sv;

		switch (obj)
		{
		case Game::TLVType::UINT32: str = "UINT32"sv; break;
		case Game::TLVType::UINT32_ARRAY: str = "UINT32_ARRAY"sv; break;
		case Game::TLVType::STRING: str = "STRING"sv; break;
		case Game::TLVType::BYTE_ARRAY: str = "BYTE_ARRAY"sv; break;
		case Game::TLVType::TEXTURE_FORMAT: str = "TEXTURE_FORMAT"sv; break;
		case Game::TLVType::TEXTURE_USAGE: str = "TEXTURE_USAGE"sv; break;
		case Game::TLVType::VERTEX_DATA: str = "VERTEX_DATA"sv; break;
		case Game::TLVType::VERTEX_DATA_ARRAY: str = "VERTEX_DATA_ARRAY"sv; break;

		case Game::TLVType::TEXTURE_DESCRIPTION: str = "TEXTURE_DESCRIPTION"sv; break;
		case Game::TLVType::MESH_DATA: str = "MESH_DATA"sv; break;
		}

		return std::format_to(ctx.out(), "{}", str);
	}
};
