#pragma once

#include "Texture.h"

#include <cstdint>
#include <cstddef>
#include <string>
#include <span>
#include <vector>

namespace Game {

	enum class TLVType : std::uint32_t
	{
		UINT32,
		STRING,
		BYTE_ARRAY,
		TEXTURE_FORMAT,
		TEXTURE_USAGE,

		TEXTURE_DESCRIPTION
	};

	class TLVEntry
	{
	public:
		TLVEntry(TLVType type, std::span<const std::byte> value);

		std::uint32_t uint32Value() const;
		std::string stringValue() const;
		std::vector<std::byte> byteArrayValue() const;
		TextureFormat textureFormatValue() const;
		TextureUsage textureUsageValue() const;
		TextureDescription textureDescriptionValue() const;
		bool IsTexture(std::string_view name) const;

		TLVType Type() const;
		std::uint32_t Size() const;

	private:
		TLVType m_Type;
		std::span<const std::byte> m_Value;
	};

}
