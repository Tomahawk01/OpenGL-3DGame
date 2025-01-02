#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"
#include "Buffer.h"
#include "MeshLoader.h"

namespace Game {

	class Mesh
	{
	public:
		Mesh(const MeshData& data);

		void Bind() const;
		void UnBind() const;

		std::uint32_t IndexCount() const;
		std::uintptr_t IndexOffset() const;

	private:
		AutoRelease<::GLuint> m_VAO;
		Buffer m_VBO;

		std::uint32_t m_IndexCount;
		std::uintptr_t m_IndexOffset;
	};

}
