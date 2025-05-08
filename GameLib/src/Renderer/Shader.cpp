#include "Shader.h"

#include <format>

namespace {

	GLenum ToNative(Game::ShaderType type)
	{
		switch (type)
		{
		case Game::ShaderType::VERTEX: return GL_VERTEX_SHADER;
		case Game::ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
		}

		throw Game::Exception("Unknown shader type: {}", std::to_underlying(type));
	}

}

namespace Game {

	Shader::Shader(std::string_view src, ShaderType type)
		:m_Handle{}, m_Type(type)
	{
		m_Handle = AutoRelease<::GLuint>{
			::glCreateShader(ToNative(type)),
			::glDeleteShader
		};

		const ::GLchar* strings[] = { src.data() };
		const ::GLint length[] = { static_cast<::GLint>(src.length()) };

		::glShaderSource(m_Handle, 1, strings, length);
		::glCompileShader(m_Handle);

		::GLint result{};
		::glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &result);

		if (result != GL_TRUE)
		{
			char log[512];
			::glGetShaderInfoLog(m_Handle, sizeof(log), nullptr, log);

			Ensure(result, "Failed to compile shader {}\n{}", m_Type, log);
		}
	}

	ShaderType Shader::GetType() const
	{
		return m_Type;
	}

	::GLuint Shader::GetNativeHandle() const
	{
		return m_Handle;
	}

	std::string to_string(ShaderType obj)
	{
		switch (obj)
		{
		case ShaderType::VERTEX: return "VERTEX";
		case ShaderType::FRAGMENT: return "FRAGMENT";
		}

		throw Exception("Unknown shader type: {}", std::to_underlying(obj));
	}

}
