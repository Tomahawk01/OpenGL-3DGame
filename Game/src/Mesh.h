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

	private:
		AutoRelease<::GLuint> m_VAO;
		AutoRelease<::GLuint> m_VBO;
	};

}
