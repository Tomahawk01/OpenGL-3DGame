#pragma once

#include "TLVEntry.h"

#include <iterator>

namespace Game {

	class TLVReader
	{
	public:
		class Iterator
		{
		public:
			using difference_type = std::ptrdiff_t;
			using value_type = TLVEntry;

			Iterator() = default;
			Iterator(std::span<const std::byte>	buffer);

			value_type operator*() const;

			Iterator& operator++();
			Iterator operator++(int);

			bool operator==(const Iterator& other) const;

		private:
			std::span<const std::byte> m_Buffer;
		};
		static_assert(std::forward_iterator<Iterator>);

		TLVReader(std::span<const std::byte> buffer);

		Iterator begin(this auto&& self)
		{
			return { self.m_Buffer };
		}

		Iterator end(this auto&& self)
		{
			return { {self.m_Buffer.data() + self.m_Buffer.size(), self.m_Buffer.data() + self.m_Buffer.size()} };
		}

	private:
		std::span<const std::byte> m_Buffer;
	};

}
