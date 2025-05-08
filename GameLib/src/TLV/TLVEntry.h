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

using namespace std::literals;

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

	std::string to_string(const TLVType& obj);

}
