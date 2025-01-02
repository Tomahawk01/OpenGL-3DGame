#include "TLVReader.h"

#include "Utilities/Error.h"

namespace Game {

	TLVReader::TLVReader(std::span<const std::byte> buffer)
		: m_Buffer(buffer)
	{}

	TLVReader::Iterator::Iterator(std::span<const std::byte> buffer)
		: m_Buffer(buffer)
	{}

	TLVReader::Iterator::value_type TLVReader::Iterator::operator*() const
	{
		Ensure(m_Buffer.size() >= sizeof(TLVType) + sizeof(std::uint32_t), "Invalid entry size");

		TLVType type{};
		std::memcpy(&type, m_Buffer.data(), sizeof(type));

		std::uint32_t length{};
		std::memcpy(&length, m_Buffer.data() + sizeof(type), sizeof(length));

		return { type, m_Buffer.subspan(sizeof(type) + sizeof(length), length) };
	}

	TLVReader::Iterator& TLVReader::Iterator::operator++()
	{
		const auto entry = operator*();
		m_Buffer = m_Buffer.last(m_Buffer.size() - entry.Size());
		return *this;
	}

	TLVReader::Iterator TLVReader::Iterator::operator++(int)
	{
		auto temp = *this;
		++*this;
		return temp;
	}

	bool TLVReader::Iterator::operator==(const Iterator& other) const
	{
		return (other.m_Buffer.data() == m_Buffer.data()) && (other.m_Buffer.size() == m_Buffer.size());
	}

}
