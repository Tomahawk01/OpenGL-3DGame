#include "Mesh.h"

#include "Math/VertexData.h"

namespace {

	static constexpr Game::VertexData vertex_data[] = {
		{.position = { 0.0f, 0.5f, 0.0f}, .color = {1.0f, 0.0f, 0.0f}},
		{.position = { -0.5f, -0.5f, 0.0f}, .color = {0.0f, 1.0f, 0.0f}},
		{.position = { 0.5f, -0.5f, 0.0f}, .color = {0.0f, 0.0f, 1.0f}}
	};

}

namespace Game {

	Mesh::Mesh()
		: m_VAO{ 0u, [](auto vao) { ::glDeleteVertexArrays(1, &vao); } }
	, m_VBO{ 0u, [](auto vbo) { ::glDeleteBuffers(1, &vbo); } }
	{
		::glGenVertexArrays(1, &m_VAO);
		::glGenBuffers(1, &m_VBO);

		::glBindVertexArray(m_VAO);

		::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

		::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
		::glEnableVertexAttribArray(0);
		::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
		::glEnableVertexAttribArray(1);

		::glBindVertexArray(0);
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
