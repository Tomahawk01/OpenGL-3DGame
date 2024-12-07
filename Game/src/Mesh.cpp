#include "Mesh.h"

#include "Math/VertexData.h"

namespace {

	static constexpr Game::VertexData vertex_data[] = {
		{.position = { -1.0f, -1.0f, -1.0f }, .color = { 0.85f, 0.22f, 0.47f }},
		{.position = { 1.0f, 1.0f, -1.0f }, .color = { 0.85f, 0.22f, 0.47f }},
		{.position = { 1.0f, -1.0f, -1.0f }, .color = { 0.85f, 0.22f, 0.47f }},
		{.position = { -1.0f, -1.0f, -1.0f }, .color = { 0.85f, 0.22f, 0.47f }},
		{.position = { -1.0f, 1.0f, -1.0f }, .color = { 0.85f, 0.22f, 0.47f }},
		{.position = { 1.0f, 1.0f, -1.0f }, .color = { 0.85f, 0.22f, 0.47f }},

		{.position = { -1.0f, -1.0f, 1.0f }, .color = { 0.33f, 0.76f, 0.87f }},
		{.position = { 1.0f, -1.0f, 1.0f }, .color = { 0.33f, 0.76f, 0.87f }},
		{.position = { 1.0f, 1.0f, 1.0f }, .color = { 0.33f, 0.76f, 0.87f }},
		{.position = { -1.0f, -1.0f, 1.0f }, .color = { 0.33f, 0.76f, 0.87f }},
		{.position = { 1.0f, 1.0f, 1.0f }, .color = { 0.33f, 0.76f, 0.87f }},
		{.position = { -1.0f, 1.0f, 1.0f }, .color = { 0.33f, 0.76f, 0.87f }},

		{.position = { -1.0f, -1.0f, -1.0f }, .color = { 0.29f, 0.68f, 0.31f }},
		{.position = { -1.0f, 1.0f, 1.0f }, .color = { 0.29f, 0.68f, 0.31f }},
		{.position = { -1.0f, -1.0f, 1.0f }, .color = { 0.29f, 0.68f, 0.31f }},
		{.position = { -1.0f, -1.0f, -1.0f }, .color = { 0.29f, 0.68f, 0.31f }},
		{.position = { -1.0f, 1.0f, -1.0f }, .color = { 0.29f, 0.68f, 0.31f }},
		{.position = { -1.0f, 1.0f, 1.0f }, .color = { 0.29f, 0.68f, 0.31f }},

		{.position = { 1.0f, -1.0f, -1.0f }, .color = { 0.91f, 0.43f, 0.15f }},
		{.position = { 1.0f, -1.0f, 1.0f }, .color = { 0.91f, 0.43f, 0.15f }},
		{.position = { 1.0f, 1.0f, 1.0f }, .color = { 0.91f, 0.43f, 0.15f }},
		{.position = { 1.0f, -1.0f, -1.0f }, .color = { 0.91f, 0.43f, 0.15f }},
		{.position = { 1.0f, 1.0f, 1.0f }, .color = { 0.91f, 0.43f, 0.15f }},
		{.position = { 1.0f, 1.0f, -1.0f }, .color = { 0.91f, 0.43f, 0.15f }},

		{.position = { -1.0f, 1.0f, -1.0f }, .color = { 0.56f, 0.34f, 0.82f }},
		{.position = { 1.0f, 1.0f, 1.0f }, .color = { 0.56f, 0.34f, 0.82f }},
		{.position = { 1.0f, 1.0f, -1.0f }, .color = { 0.56f, 0.34f, 0.82f }},
		{.position = { -1.0f, 1.0f, -1.0f }, .color = { 0.56f, 0.34f, 0.82f }},
		{.position = { -1.0f, 1.0f, 1.0f }, .color = { 0.56f, 0.34f, 0.82f }},
		{.position = { -1.0f, 1.0f, 1.0f }, .color = { 0.56f, 0.34f, 0.82f }},

		{.position = { -1.0f, -1.0f, -1.0f }, .color = { 0.94f, 0.91f, 0.17f }},
		{.position = { 1.0f, -1.0f, 1.0f }, .color = { 0.94f, 0.91f, 0.17f }},
		{.position = { 1.0f, -1.0f, -1.0f }, .color = { 0.94f, 0.91f, 0.17f }},
		{.position = { -1.0f, -1.0f, -1.0f }, .color = { 0.94f, 0.91f, 0.17f }},
		{.position = { -1.0f, -1.0f, 1.0f }, .color = { 0.94f, 0.91f, 0.17f }},
		{.position = { 1.0f, -1.0f, 1.0f }, .color = { 0.94f, 0.91f, 0.17f }}
	};

}

namespace Game {

	Mesh::Mesh()
		: m_VAO{ 0u, [](auto vao) { ::glDeleteVertexArrays(1, &vao); } }
		, m_VBO{ 0u, [](auto vbo) { ::glDeleteBuffers(1, &vbo); } }
	{
		::glCreateBuffers(1, &m_VBO);
		::glNamedBufferStorage(m_VBO, sizeof(vertex_data), vertex_data, GL_DYNAMIC_STORAGE_BIT);

		::glCreateVertexArrays(1, &m_VAO);

		::glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(VertexData));

		::glEnableVertexArrayAttrib(m_VAO, 0);
		::glEnableVertexArrayAttrib(m_VAO, 1);

		::glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, position));
		::glVertexArrayAttribFormat(m_VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, color));

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

}
