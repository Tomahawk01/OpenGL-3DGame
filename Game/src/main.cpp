#include "Error.h"
#include "Exception.h"
#include "Material.h"
#include "OpenGL.h"
#include "Shader.h"
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

		Game::Shader vertexShader = Game::Shader{ vertex_shader_src, Game::ShaderType::VERTEX };
		Game::Shader fragmentShader = Game::Shader{ fragment_shader_src, Game::ShaderType::FRAGMENT };
		Game::Material material = Game::Material{ vertexShader, fragmentShader };

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

			::glUseProgram(material.NativeHandle());
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
