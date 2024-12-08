#include "Mesh.h"

#include "Math/VertexData.h"
#include "BufferWriter.h"

#include <ranges>

namespace {

	constexpr Game::VertexData vertex_data[] = {
		// Front face
		{ .position = {-0.5f, -0.5f, -0.5f}, .uv = {0.0f, 0.0f} },
		{ .position = {0.5f,  -0.5f, -0.5f}, .uv = {1.0f, 0.0f} },
		{ .position = {0.5f,  0.5f,  -0.5f}, .uv = {1.0f, 1.0f} },
		{ .position = {-0.5f, 0.5f,  -0.5f}, .uv = {0.0f, 1.0f} },

		// Back face
		{ .position = {-0.5f, -0.5f,  0.5f}, .uv = {1.0f, 0.0f} },
		{ .position = {0.5f,  -0.5f,  0.5f}, .uv = {0.0f, 0.0f} },
		{ .position = {0.5f,  0.5f,   0.5f}, .uv = {0.0f, 1.0f} },
		{ .position = {-0.5f, 0.5f,   0.5f}, .uv = {1.0f, 1.0f} },

		// Left face
		{ .position = {-0.5f, -0.5f,  0.5f}, .uv = {0.0f, 0.0f} },
		{ .position = {-0.5f, -0.5f, -0.5f}, .uv = {1.0f, 0.0f} },
		{ .position = {-0.5f,  0.5f, -0.5f}, .uv = {1.0f, 1.0f} },
		{ .position = {-0.5f,  0.5f,  0.5f}, .uv = {0.0f, 1.0f} },

		// Right face
		{ .position = {0.5f,  -0.5f, -0.5f}, .uv = {0.0f, 0.0f} },
		{ .position = {0.5f,  -0.5f,  0.5f}, .uv = {1.0f, 0.0f} },
		{ .position = {0.5f,   0.5f,  0.5f}, .uv = {1.0f, 1.0f} },
		{ .position = {0.5f,   0.5f, -0.5f}, .uv = {0.0f, 1.0f} },

		// Top face
		{ .position = {-0.5f,  0.5f, -0.5f}, .uv = {0.0f, 0.0f} },
		{ .position = {0.5f,   0.5f, -0.5f}, .uv = {1.0f, 0.0f} },
		{ .position = {0.5f,   0.5f,  0.5f}, .uv = {1.0f, 1.0f} },
		{ .position = {-0.5f,  0.5f,  0.5f}, .uv = {0.0f, 1.0f} },

		// Bottom face
		{ .position = {-0.5f, -0.5f,  0.5f}, .uv = {0.0f, 0.0f} },
		{ .position = {0.5f,  -0.5f,  0.5f}, .uv = {1.0f, 0.0f} },
		{ .position = {0.5f,  -0.5f, -0.5f}, .uv = {1.0f, 1.0f} },
		{ .position = {-0.5f, -0.5f, -0.5f}, .uv = {0.0f, 1.0f} }
	};

	constexpr GLuint indices[] = {
		// Front face
		0, 1, 2, 2, 3, 0,
		// Back face
		4, 5, 6, 6, 7, 4,
		// Left face
		8, 9, 10, 10, 11, 8,
		// Right face
		12, 13, 14, 14, 15, 12,
		// Top face
		16, 17, 18, 18, 19, 16,
		// Bottom face
		20, 21, 22, 22, 23, 20
	};

}

namespace Game {

	Mesh::Mesh()
		: m_VAO{ 0u, [](auto vao) { ::glDeleteVertexArrays(1, &vao); } }
		, m_VBO{ sizeof(vertex_data) + sizeof(indices) }
		, m_IndexCount(static_cast<std::uint32_t>(std::ranges::distance(indices)))
		, m_IndexOffset(sizeof(vertex_data))
	{
		{
			BufferWriter writer{ m_VBO };
			writer.Write(vertex_data);
			writer.Write(indices);
		}

		::glCreateVertexArrays(1, &m_VAO);
		::glVertexArrayVertexBuffer(m_VAO, 0, m_VBO.GetNativeHandle(), 0, sizeof(VertexData));
		::glVertexArrayElementBuffer(m_VAO, m_VBO.GetNativeHandle());

		::glEnableVertexArrayAttrib(m_VAO, 0);
		::glEnableVertexArrayAttrib(m_VAO, 1);

		::glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, position));
		::glVertexArrayAttribFormat(m_VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, uv));

		::glVertexArrayAttribBinding(m_VAO, 0, 0);
		::glVertexArrayAttribBinding(m_VAO, 1, 0);
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
