#pragma once

#include "AutoRelease.h"
#include "OpenGL.h"
#include "Shader.h"

namespace Game {

	class Material
	{
	public:
		Material(const Shader& vertexShader, const Shader& fragmentShader);

		::GLuint NativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
	};

}
