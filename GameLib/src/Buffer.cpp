#include "Buffer.h"

#include "Utilities/Error.h"

namespace Game {

	Buffer::Buffer(uint32_t size)
		: m_Buffer{ 0u, [](auto buffer) { ::glDeleteBuffers(1, &buffer); } }
		, m_Size{ size }
	{
		::glCreateBuffers(1, &m_Buffer);
		::glNamedBufferStorage(m_Buffer, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	void Buffer::Write(std::span<const std::byte> data, size_t offset) const
	{
		Ensure(m_Size >= data.size_bytes() + offset, "Buffer is too small");
		::glNamedBufferSubData(m_Buffer, offset, data.size(), data.data());
	}

	GLuint Buffer::GetNativeHandle() const
	{
		return m_Buffer;
	}

}
