#include "Shader.h"

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
		m_Handle = Game::AutoRelease<::GLuint>{
			::glCreateShader(ToNative(type)),
			::glDeleteShader
		};

		const ::GLchar* strings[] = { src.data() };
		const ::GLint length[] = { static_cast<::GLint>(src.length()) };

		::glShaderSource(m_Handle, 1, strings, length);
		::glCompileShader(m_Handle);

		::GLint result{};
		::glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &result);
		Game::Ensure(result, "Failed to compile shader {}", m_Type);
	}

	ShaderType Shader::Type() const
	{
		return m_Type;
	}

	::GLuint Shader::NativeHandle() const
	{
		return m_Handle;
	}

}
