#pragma once

#include "Buffer.h"

namespace Game {

	class BufferWriter
	{
	public:
		BufferWriter(const Buffer& buffer)
			: m_Buffer(buffer)
			, m_Offset{}
		{}

		template<class T, std::size_t N>
		void Write(const T(&data)[N])
		{
			m_Buffer.Write({ reinterpret_cast<const std::byte*>(data), sizeof(T) * N }, m_Offset);
			m_Offset += sizeof(T) * N;
		}

		template<class T>
		void Write(std::span<const T> data)
		{
			m_Buffer.Write(std::as_bytes(data), m_Offset);
			m_Offset += data.size_bytes();
		}

	private:
		const Buffer& m_Buffer;
		std::size_t m_Offset;
	};

}
