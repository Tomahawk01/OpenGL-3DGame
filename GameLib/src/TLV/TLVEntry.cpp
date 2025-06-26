#include "TLVEntry.h"

#include "TLVReader.h"

#include "Utilities/Error.h"

namespace Game {

	TLVEntry::TLVEntry(TLVType type, std::span<const std::byte> value)
		: m_Type(type)
		, m_Value(value)
	{}

	uint32_t TLVEntry::uint32Value() const
	{
		Ensure(m_Type == TLVType::UINT32, "Incorrect type");
		Ensure(m_Value.size() == sizeof(uint32_t), "Incorrect size");

		uint32_t value{};
		std::memcpy(&value, m_Value.data(), sizeof(value));

		return value;
	}

	std::vector<uint32_t> TLVEntry::uint32ArrayValue() const
	{
		Ensure(m_Type == TLVType::UINT32_ARRAY, "Incorrect type");

		auto value = std::vector<uint32_t>(m_Value.size() / sizeof(uint32_t));
		std::memcpy(value.data(), m_Value.data(), m_Value.size());

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
		if (m_Type != TLVType::TEXTURE_DESCRIPTION)
			return false;

		TLVReader reader{ m_Value };
		TLVReader::Iterator readerCursor = std::ranges::begin(reader);

		const auto textureName = (*readerCursor).stringValue();

		return textureName == name;
	}

	VertexData TLVEntry::vertexDataValue() const
	{
		Ensure(m_Type == TLVType::VERTEX_DATA, "Incorrect type");
		Ensure(m_Value.size() == sizeof(VertexData), "Incorrect size");

		VertexData value{};
		std::memcpy(&value, m_Value.data(), sizeof(value));

		return value;
	}

	std::vector<VertexData> TLVEntry::vertexDataArrayValue() const
	{
		Ensure(m_Type == TLVType::VERTEX_DATA_ARRAY, "Incorrect type");

		auto value = std::vector<VertexData>(m_Value.size() / sizeof(VertexData));
		std::memcpy(value.data(), m_Value.data(), m_Value.size());

		return value;
	}

	MeshData TLVEntry::meshValue() const
	{
		Ensure(m_Type == TLVType::MESH_DATA, "Incorrect type");

		const TLVReader reader{ m_Value };
		TLVReader::Iterator readerCursor = std::ranges::begin(reader);

		Ensure((*readerCursor).Type() == TLVType::STRING, "First member not string");
		readerCursor++;

		Ensure((*readerCursor).Type() == TLVType::VERTEX_DATA_ARRAY, "Second member not vertex data array");
		const std::span<const VertexData> vertexData{ reinterpret_cast<const VertexData*>((*readerCursor).m_Value.data()), (*readerCursor).m_Value.size() / sizeof(VertexData) };
		readerCursor++;

		Ensure((*readerCursor).Type() == TLVType::UINT32_ARRAY, "Third member not uint32 array");
		const std::span<const uint32_t> indexData{ reinterpret_cast<const uint32_t*>((*readerCursor).m_Value.data()), (*readerCursor).m_Value.size() / sizeof(uint32_t) };
		readerCursor++;

		Ensure(readerCursor == std::ranges::end(reader), "Mesh data TLV is too large");

		return { vertexData, indexData };
	}

	bool TLVEntry::IsMesh(std::string_view name) const
	{
		if (m_Type != TLVType::MESH_DATA)
			return false;

		const TLVReader reader{ m_Value };
		TLVReader::Iterator readerCursor = std::ranges::begin(reader);

		const std::string meshName = (*readerCursor).stringValue();

		return meshName == name;
	}

	TextFile TLVEntry::textFileValue() const
	{
		Ensure(m_Type == TLVType::TEXT_FILE, "Incorrect type");

		const TLVReader reader{ m_Value };
		TLVReader::Iterator readerCursor = std::ranges::begin(reader);

		const std::string name = (*readerCursor).stringValue();
		readerCursor++;
		const std::string data = (*readerCursor).stringValue();

		readerCursor++;
		Ensure(readerCursor == std::ranges::end(reader), "Text file TLV is too large");

		return { name, data };
	}

	bool TLVEntry::IsTextFile(std::string_view name) const
	{
		if (m_Type != TLVType::TEXT_FILE)
			return false;

		const TLVReader reader{ m_Value };
		TLVReader::Iterator readerCursor = std::ranges::begin(reader);

		const std::string textFileName = (*readerCursor).stringValue();

		return textFileName == name;
	}

	std::string to_string(const TLVType& obj)
	{
		auto str = "unknown"sv;

		switch (obj)
		{
		case TLVType::UINT32: str = "UINT32"sv; break;
		case TLVType::UINT32_ARRAY: str = "UINT32_ARRAY"sv; break;
		case TLVType::STRING: str = "STRING"sv; break;
		case TLVType::BYTE_ARRAY: str = "BYTE_ARRAY"sv; break;
		case TLVType::TEXTURE_FORMAT: str = "TEXTURE_FORMAT"sv; break;
		case TLVType::TEXTURE_USAGE: str = "TEXTURE_USAGE"sv; break;
		case TLVType::VERTEX_DATA: str = "VERTEX_DATA"sv; break;
		case TLVType::VERTEX_DATA_ARRAY: str = "VERTEX_DATA_ARRAY"sv; break;

		case TLVType::TEXTURE_DESCRIPTION: str = "TEXTURE_DESCRIPTION"sv; break;
		case TLVType::MESH_DATA: str = "MESH_DATA"sv; break;
		case TLVType::TEXT_FILE: str = "TEXT_FILE"sv; break;
		}

		return std::format("{}", str);
	}

	TLVType TLVEntry::Type() const
	{
		return m_Type;
	}

	uint32_t TLVEntry::Size() const
	{
		return static_cast<uint32_t>(sizeof(m_Type) + sizeof(uint32_t) + m_Value.size());
	}

}
