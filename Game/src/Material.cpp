#include "Material.h"

namespace Game {

	Material::Material(const Shader& vertexShader, const Shader& fragmentShader)
		: m_Handle{}
	{
		Ensure(vertexShader.GetType() == ShaderType::VERTEX, "Shader is not a vertex shader");
		Ensure(fragmentShader.GetType() == ShaderType::FRAGMENT, "Shader is not a fragment shader");

		m_Handle = AutoRelease<::GLuint>{
			::glCreateProgram(),
			::glDeleteProgram
		};
		Ensure(m_Handle, "Failed to create OpenGL program");

		::glAttachShader(m_Handle, vertexShader.GetNativeHandle());
		::glAttachShader(m_Handle, fragmentShader.GetNativeHandle());
		::glLinkProgram(m_Handle);

		::GLint result{};
		::glGetProgramiv(m_Handle, GL_LINK_STATUS, &result);

		if (result != GL_TRUE)
		{
			char log[512];
			::glGetProgramInfoLog(m_Handle, sizeof(log), nullptr, log);

			Game::Ensure(result, "Failed to link program\n{}", log);
		}
	}

	::GLuint Material::GetNativeHandle() const
	{
		return m_Handle;
	}

}
