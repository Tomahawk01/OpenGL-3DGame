#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"

namespace Game {

	class Mesh
	{
	public:
		Mesh();

		void Bind() const;
		void UnBind() const;

		std::uint32_t IndexCount() const;
		std::uintptr_t IndexOffset() const;

	private:
		AutoRelease<::GLuint> m_VAO;
		AutoRelease<::GLuint> m_VBO;

		std::uint32_t m_IndexCount;
		std::uintptr_t m_IndexOffset;
	};

}
