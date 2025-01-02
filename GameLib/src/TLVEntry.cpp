#include "TLVEntry.h"

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

	TLVType TLVEntry::Type() const
	{
		return m_Type;
	}

	std::uint32_t TLVEntry::Size() const
	{
		return static_cast<std::uint32_t>(sizeof(m_Type) + sizeof(std::uint32_t) + m_Value.size());
	}

}
