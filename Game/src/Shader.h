#pragma once

#include "AutoRelease.h"
#include "Error.h"
#include "Exception.h"
#include "OpenGL.h"

#include <format>
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

		ShaderType Type() const;
		::GLuint NativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
		ShaderType m_Type;
	};

}

template<>
struct std::formatter<Game::ShaderType>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::ShaderType& obj, std::format_context& ctx) const
	{
		switch (obj)
		{
		case Game::ShaderType::VERTEX: return std::format_to(ctx.out(), "VERTEX");
		case Game::ShaderType::FRAGMENT: return std::format_to(ctx.out(), "FRAGMENT");
		}

		throw Game::Exception("Unknown shader type: {}", std::to_underlying(obj));
	}
};