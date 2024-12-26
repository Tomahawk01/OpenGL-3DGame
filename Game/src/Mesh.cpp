#include "Mesh.h"

#include "Math/VertexData.h"
#include "BufferWriter.h"

#include <ranges>

namespace Game {

	Mesh::Mesh(const MeshData& data)
		: m_VAO{ 0u, [](auto vao) { ::glDeleteVertexArrays(1, &vao); } }
		, m_VBO{ static_cast<std::uint32_t>(data.vertices.size_bytes() + data.indices.size_bytes()) }
		, m_IndexCount(static_cast<std::uint32_t>(data.indices.size()))
		, m_IndexOffset(data.vertices.size_bytes())
	{
		{
			BufferWriter writer{ m_VBO };
			writer.Write(data.vertices);
			writer.Write(data.indices);
		}

		::glCreateVertexArrays(1, &m_VAO);
		::glVertexArrayVertexBuffer(m_VAO, 0, m_VBO.GetNativeHandle(), 0, sizeof(VertexData));
		::glVertexArrayElementBuffer(m_VAO, m_VBO.GetNativeHandle());

		::glEnableVertexArrayAttrib(m_VAO, 0);
		::glEnableVertexArrayAttrib(m_VAO, 1);
		::glEnableVertexArrayAttrib(m_VAO, 2);

		::glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, position));
		::glVertexArrayAttribFormat(m_VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, normal));
		::glVertexArrayAttribFormat(m_VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, uv));

		::glVertexArrayAttribBinding(m_VAO, 0, 0);
		::glVertexArrayAttribBinding(m_VAO, 1, 0);
		::glVertexArrayAttribBinding(m_VAO, 2, 0);
	}

	void Mesh::Bind() const
	{
		::glBindVertexArray(m_VAO);
	}

	void Mesh::UnBind() const
	{
		::glBindVertexArray(0);
	}

	std::uint32_t Mesh::IndexCount() const
	{
		return m_IndexCount;
	}

	std::uintptr_t Mesh::IndexOffset() const
	{
		return m_IndexOffset;
	}

}
