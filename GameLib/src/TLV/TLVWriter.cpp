#include "TLVWriter.h"

#include "TLVEntry.h"

#include <ranges>

namespace {

	void WriteBytes(std::vector<std::byte>& buffer, std::span<const std::byte> data)
	{
		buffer.insert(std::ranges::end(buffer), std::ranges::cbegin(data), std::ranges::cend(data));
	}

	void WriteEntry(std::vector<std::byte>& buffer, Game::TLVType type, uint32_t length, std::span<const std::byte> value)
	{
		WriteBytes(buffer, { reinterpret_cast<const std::byte*>(&type), sizeof(type) });
		WriteBytes(buffer, { reinterpret_cast<const std::byte*>(&length), sizeof(length) });
		WriteBytes(buffer, value);
	}

}

namespace Game {

	std::vector<std::byte> TLVWriter::yield()
	{
		std::vector<std::byte> temp{};
		std::ranges::swap(temp, m_Buffer);
		return temp;
	}

	void TLVWriter::Write(uint32_t value)
	{
		const auto type = TLVType::UINT32;
		const auto length = sizeof(value);
		const std::span<const std::byte> valueBytes{ reinterpret_cast<const std::byte*>(&value), length };

		WriteEntry(m_Buffer, type, length, valueBytes);
	}

	void TLVWriter::Write(std::span<const uint32_t> value)
	{
		const auto type = TLVType::UINT32_ARRAY;
		const auto length = static_cast<uint32_t>(value.size() * sizeof(uint32_t));
		const std::span<const std::byte> valueBytes{ reinterpret_cast<const std::byte*>(value.data()), length };

		WriteEntry(m_Buffer, type, length, valueBytes);
	}

	void TLVWriter::Write(std::string_view value)
	{
		const auto type = TLVType::STRING;
		const auto length = static_cast<uint32_t>(value.length());
		const std::span<const std::byte> valueBytes{ reinterpret_cast<const std::byte*>(value.data()), length };

		WriteEntry(m_Buffer, type, length, valueBytes);
	}

	void TLVWriter::Write(std::span<const std::byte> value)
	{
		const auto type = TLVType::BYTE_ARRAY;
		const auto length = static_cast<uint32_t>(value.size());
		const std::span<const std::byte> valueBytes{ reinterpret_cast<const std::byte*>(value.data()), length };

		WriteEntry(m_Buffer, type, length, valueBytes);
	}

	void TLVWriter::Write(TextureFormat value)
	{
		const auto type = TLVType::TEXTURE_FORMAT;
		const auto length = sizeof(value);
		const std::span<const std::byte> valueBytes{ reinterpret_cast<const std::byte*>(&value), length };

		WriteEntry(m_Buffer, type, length, valueBytes);
	}

	void TLVWriter::Write(TextureUsage value)
	{
		const auto type = TLVType::TEXTURE_USAGE;
		const auto length = sizeof(value);
		const std::span<const std::byte> valueBytes{ reinterpret_cast<const std::byte*>(&value), length };

		WriteEntry(m_Buffer, type, length, valueBytes);
	}

	void TLVWriter::Write(const VertexData& value)
	{
		const auto type = TLVType::VERTEX_DATA;
		const auto length = sizeof(value);
		const std::span<const std::byte> valueBytes{ reinterpret_cast<const std::byte*>(&value), length };

		WriteEntry(m_Buffer, type, length, valueBytes);
	}

	void TLVWriter::Write(std::span<const VertexData> value)
	{
		const auto type = TLVType::VERTEX_DATA_ARRAY;
		const auto length = static_cast<uint32_t>(value.size() * sizeof(VertexData));
		const std::span<const std::byte> valueBytes{ reinterpret_cast<const std::byte*>(value.data()), length };

		WriteEntry(m_Buffer, type, length, valueBytes);
	}

	void TLVWriter::Write(
		std::string_view name,
		uint32_t width,
		uint32_t height,
		TextureFormat format,
		TextureUsage usage,
		std::span<const std::byte> data)
	{
		TLVWriter writer{};

		writer.Write(name);
		writer.Write(width);
		writer.Write(height);
		writer.Write(format);
		writer.Write(usage);
		writer.Write(data);

		const auto value = writer.yield();
		const auto type = TLVType::TEXTURE_DESCRIPTION;
		const auto length = static_cast<uint32_t>(value.size());

		WriteEntry(m_Buffer, type, length, value);
	}

	void TLVWriter::Write(std::string_view name, std::span<const VertexData> vertices, std::span<const uint32_t> indices)
	{
		TLVWriter writer{};

		writer.Write(name);
		writer.Write(vertices);
		writer.Write(indices);

		const auto value = writer.yield();
		const auto type = TLVType::MESH_DATA;
		const auto length = static_cast<uint32_t>(value.size());

		WriteEntry(m_Buffer, type, length, value);
	}

	void TLVWriter::Write(std::string_view name, std::string_view data)
	{
		TLVWriter writer{};

		writer.Write(name);
		writer.Write(data);

		const auto value = writer.yield();
		const auto type = TLVType::TEXT_FILE;
		const auto length = static_cast<uint32_t>(value.size());

		WriteEntry(m_Buffer, type, length, value);
	}

	void TLVWriter::Write(std::string_view name, std::span<const std::byte> format, std::span<const std::byte> data)
	{
		TLVWriter writer{};

		writer.Write(name);
		writer.Write(format);
		writer.Write(data);

		const auto value = writer.yield();
		const auto type = TLVType::SOUND_DATA;
		const auto length = static_cast<uint32_t>(value.size());

		WriteEntry(m_Buffer, type, length, value);
	}

}
