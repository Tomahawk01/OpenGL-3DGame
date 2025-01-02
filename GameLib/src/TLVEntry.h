#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <span>

namespace Game {

	enum class TLVType : std::uint32_t
	{
		UINT32,
		STRING
	};

	class TLVEntry
	{
	public:
		TLVEntry(TLVType type, std::span<const std::byte> value);

		std::uint32_t uint32Value() const;
		std::string stringValue() const;

		TLVType Type() const;
		std::uint32_t Size() const;

	private:
		TLVType m_Type;
		std::span<const std::byte> m_Value;
	};

}
