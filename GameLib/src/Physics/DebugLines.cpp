#include "DebugLines.h"

#include "BufferWriter.h"

namespace Game {

	DebugLines::DebugLines(std::span<const LineData> lines)
		: m_VBO{ static_cast<uint32_t>(lines.size_bytes()) }
		, m_VAO{ 0u, [](auto vao) { ::glDeleteVertexArrays(1, &vao); } }
		, m_Count(static_cast<uint32_t>(lines.size()))
	{
		{
			BufferWriter writer{ m_VBO };
			writer.Write(lines);
		}

		::glCreateVertexArrays(1, &m_VAO);
		::glVertexArrayVertexBuffer(m_VAO, 0, m_VBO.GetNativeHandle(), 0, sizeof(LineData));

		::glEnableVertexArrayAttrib(m_VAO, 0);
		::glEnableVertexArrayAttrib(m_VAO, 1);

		::glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(LineData, position));
		::glVertexArrayAttribFormat(m_VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(LineData, color));

		::glVertexArrayAttribBinding(m_VAO, 0, 0);
		::glVertexArrayAttribBinding(m_VAO, 1, 0);
	}

	void DebugLines::Bind() const
	{
		::glBindVertexArray(m_VAO);
	}

	void DebugLines::UnBind() const
	{
		::glBindVertexArray(0);
	}

	uint32_t DebugLines::Count() const
	{
		return m_Count;
	}

}
