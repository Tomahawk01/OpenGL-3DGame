#include "Error.h"
#include "Exception.h"
#include "OpenGL.h"
#include "Window.h"

#include <iostream>
#include <print>

namespace {

	static constexpr const char* vertex_shader_src = R"(
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 vertex_color;

void main()
{
	gl_Position = vec4(position, 1.0);
	vertex_color = color;
}
	)";

	static constexpr const char* fragment_shader_src = R"(
#version 460 core

in vec3 vertex_color;
out vec4 frag_color;

void main()
{
	frag_color = vec4(vertex_color, 1.0);
}
	)";

	Game::AutoRelease<::GLuint> CompileShader(std::string_view src, ::GLenum shaderType)
	{
		auto shader = Game::AutoRelease<::GLuint>{
			::glCreateShader(shaderType),
			::glDeleteShader
		};

		const ::GLchar* strings[] = { src.data() };
		const ::GLint length[] = { static_cast<::GLint>(src.length()) };

		::glShaderSource(shader, 1, strings, length);
		::glCompileShader(shader);

		::GLint result{};
		::glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		Game::Ensure(result, "Failed to compile shader {}", shaderType);

		return shader;
	}
}

int main()
{
	std::println("hello world");

	static constexpr float vertex_data[] = {
		 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	try
	{
		Game::Window window{ 800u, 600u };

		auto vertexShader = CompileShader(vertex_shader_src, GL_VERTEX_SHADER);
		auto fragmentShader = CompileShader(fragment_shader_src, GL_FRAGMENT_SHADER);

		auto program = Game::AutoRelease<::GLuint>{
			::glCreateProgram(),
			::glDeleteProgram
		};
		Game::Ensure(program, "Failed to create OpenGL program");

		::glAttachShader(program, vertexShader);
		::glAttachShader(program, fragmentShader);
		::glLinkProgram(program);

		::glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		::GLuint vao{};
		::glGenVertexArrays(1, &vao);
		::GLuint vbo{};
		::glGenBuffers(1, &vbo);

		::glBindVertexArray(vao);
		::glBindBuffer(GL_ARRAY_BUFFER, vbo);
		::glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

		::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
		::glEnableVertexAttribArray(0);
		::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
		::glEnableVertexAttribArray(1);

		::glBindVertexArray(0);

		while (window.IsRunning())
		{
			::glClear(GL_COLOR_BUFFER_BIT);

			::glUseProgram(program);
			::glBindVertexArray(vao);
			::glDrawArrays(GL_TRIANGLES, 0, 3);

			window.Swap();
		}
	}
	catch (const Game::Exception& err)
	{
		std::println(std::cerr, "{}", err);
	}
	catch (...)
	{
		std::println(std::cerr, "Unknown exception");
	}

	return 0;
}
