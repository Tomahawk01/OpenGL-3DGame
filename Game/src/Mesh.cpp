#include "Mesh.h"

#include "Math/VertexData.h"

#include <ranges>

namespace {

	constexpr Game::VertexData vertex_data[] = {
		{.position = {-0.5f, -0.5f, -0.5f},	.color = {1.0f, 0.0f, 0.0f}},
		{.position = {0.5f, -0.5f, -0.5f}, .color = {0.0f, 1.0f, 0.0f}},
		{.position = {0.5f, 0.5f, -0.5f}, .color = {0.0f, 0.0f, 1.0f}},
		{.position = {-0.5f, 0.5f, -0.5f}, .color = {1.0f, 1.0f, 0.0f}},

		{.position = {-0.5f, -0.5f, 0.5f}, .color = {1.0f, 0.0f, 1.0f}},
		{.position = {0.5f, -0.5f, 0.5f}, .color = {0.0f, 1.0f, 1.0f}},
		{.position = {0.5f, 0.5f, 0.5f}, .color = {0.5f, 0.5f, 0.5f}},
		{.position = {-0.5f, 0.5f, 0.5f}, .color = {1.0f, 1.0f, 1.0f}}
	};

	constexpr GLuint indices[] = {
		// Front face
		0, 1, 2, 0, 2, 3,
		// Back face
		4, 5, 6, 4, 6, 7,
		// Left face
		0, 3, 7, 0, 7, 4,
		// Right face
		1, 2, 6, 1, 6, 5,
		// Bottom face
		0, 1, 5, 0, 5, 4,
		// Top face
		3, 2, 6, 3, 6, 7
	};

}

namespace Game {

	Mesh::Mesh()
		: m_VAO{ 0u, [](auto vao) { ::glDeleteVertexArrays(1, &vao); } }
		, m_VBO{ 0u, [](auto vbo) { ::glDeleteBuffers(1, &vbo); } }
		, m_IndexCount(static_cast<std::uint32_t>(std::ranges::distance(indices)))
		, m_IndexOffset(sizeof(vertex_data))
	{
		::glCreateBuffers(1, &m_VBO);
		::glNamedBufferStorage(m_VBO, sizeof(vertex_data) + sizeof(indices), nullptr, GL_DYNAMIC_STORAGE_BIT);
		::glNamedBufferSubData(m_VBO, 0, sizeof(vertex_data), vertex_data);
		::glNamedBufferSubData(m_VBO, sizeof(vertex_data), sizeof(indices), indices);

		::glCreateVertexArrays(1, &m_VAO);
		::glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(VertexData));
		::glVertexArrayElementBuffer(m_VAO, m_VBO);

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

	std::uint32_t Mesh::IndexCount() const
	{
		return m_IndexCount;
	}

	std::uintptr_t Mesh::IndexOffset() const
	{
		return m_IndexOffset;
	}

}
