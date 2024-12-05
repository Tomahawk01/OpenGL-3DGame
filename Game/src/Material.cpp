#include "Material.h"

namespace Game {

	Material::Material(const Shader& vertexShader, const Shader& fragmentShader)
		: m_Handle{}
	{
		Ensure(vertexShader.Type() == ShaderType::VERTEX, "Shader is not a vertex shader");
		Ensure(fragmentShader.Type() == ShaderType::FRAGMENT, "Shader is not a fragment shader");

		m_Handle = AutoRelease<::GLuint>{
			::glCreateProgram(),
			::glDeleteProgram
		};
		Ensure(m_Handle, "Failed to create OpenGL program");

		::glAttachShader(m_Handle, vertexShader.NativeHandle());
		::glAttachShader(m_Handle, fragmentShader.NativeHandle());
		::glLinkProgram(m_Handle);
	}

	::GLuint Material::NativeHandle() const
	{
		return m_Handle;
	}

}
