#pragma once

#include "Utilities/AutoRelease.h"
#include "Utilities/Error.h"
#include "Utilities/Exception.h"
#include "OpenGL.h"

#include <string_view>

namespace Game {

	enum class ShaderType
	{
		VERTEX,
		FRAGMENT
	};

	class Shader
	{
	public:
		Shader(std::string_view src, ShaderType type);

		ShaderType GetType() const;
		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
		ShaderType m_Type;
	};

	std::string to_string(ShaderType obj);

}
