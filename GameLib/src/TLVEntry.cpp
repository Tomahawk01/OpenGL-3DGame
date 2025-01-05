#include "TLVEntry.h"

#include "TLVReader.h"

#include "Utilities/Error.h"

namespace Game {

	TLVEntry::TLVEntry(TLVType type, std::span<const std::byte> value)
		: m_Type(type)
		, m_Value(value)
	{}

	std::uint32_t TLVEntry::uint32Value() const
	{
		Ensure(m_Type == TLVType::UINT32, "Incorrect type");
		Ensure(m_Value.size() == sizeof(std::uint32_t), "Incorrect size");

		std::uint32_t value{};
		std::memcpy(&value, m_Value.data(), sizeof(value));

		return value;
	}

	std::string TLVEntry::stringValue() const
	{
		Ensure(m_Type == TLVType::STRING, "Incorrect type");

		const auto* ptr = reinterpret_cast<const char*>(m_Value.data());

		return std::string(ptr, ptr + m_Value.size());
	}

	std::vector<std::byte> TLVEntry::byteArrayValue() const
	{
		Ensure(m_Type == TLVType::BYTE_ARRAY, "Incorrect type");

		const auto* ptr = reinterpret_cast<const std::byte*>(m_Value.data());

		return std::vector<std::byte>(ptr, ptr + m_Value.size());
	}

	TextureFormat TLVEntry::textureFormatValue() const
	{
		Ensure(m_Type == TLVType::TEXTURE_FORMAT, "Incorrect type");
		Ensure(m_Value.size() == sizeof(TextureFormat), "Incorrect size");

		TextureFormat value{};
		std::memcpy(&value, m_Value.data(), sizeof(value));

		return value;
	}

	TextureUsage TLVEntry::textureUsageValue() const
	{
		Ensure(m_Type == TLVType::TEXTURE_USAGE, "Incorrect type");
		Ensure(m_Value.size() == sizeof(TextureUsage), "Incorrect size");

		TextureUsage value{};
		std::memcpy(&value, m_Value.data(), sizeof(value));

		return value;
	}

	TextureDescription TLVEntry::textureDescriptionValue() const
	{
		Ensure(m_Type == TLVType::TEXTURE_DESCRIPTION, "Incorrect type");

		TLVReader reader{ m_Value };
		TLVReader::Iterator readerCursor = std::ranges::begin(reader);

		Ensure((*readerCursor).Type() == TLVType::STRING, "First member not string");
		readerCursor++;

		const auto width = (*readerCursor).uint32Value();
		readerCursor++;
		Ensure(readerCursor != std::ranges::end(reader), "Texture TLV is too small");

		const auto height = (*readerCursor).uint32Value();
		readerCursor++;
		Ensure(readerCursor != std::ranges::end(reader), "Texture TLV is too small");

		const auto format = (*readerCursor).textureFormatValue();
		readerCursor++;
		Ensure(readerCursor != std::ranges::end(reader), "Texture TLV is too small");

		const auto usage = (*readerCursor).textureUsageValue();
		readerCursor++;
		Ensure(readerCursor != std::ranges::end(reader), "Texture TLV is too small");

		auto data = (*readerCursor).byteArrayValue();
		readerCursor++;
		Ensure(readerCursor == std::ranges::end(reader), "Texture TLV is too large");

		return { width, height, format, usage, std::move(data) };
	}

	bool TLVEntry::IsTexture(std::string_view name) const
	{
		Ensure(m_Type == TLVType::TEXTURE_DESCRIPTION, "Incorrect type");

		TLVReader reader{ m_Value };
		TLVReader::Iterator readerCursor = std::ranges::begin(reader);

		const auto textureName = (*readerCursor).stringValue();

		return textureName == name;
	}

	TLVType TLVEntry::Type() const
	{
		return m_Type;
	}

	std::uint32_t TLVEntry::Size() const
	{
		return static_cast<std::uint32_t>(sizeof(m_Type) + sizeof(std::uint32_t) + m_Value.size());
	}

}
