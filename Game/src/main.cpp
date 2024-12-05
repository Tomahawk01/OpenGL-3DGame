#include "Utilities/Error.h"
#include "Utilities/Exception.h"
#include "Material.h"
#include "Mesh.h"
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

	try
	{
		Game::Window window{ 800u, 600u };

		Game::Shader vertexShader{ vertex_shader_src, Game::ShaderType::VERTEX };
		Game::Shader fragmentShader{ fragment_shader_src, Game::ShaderType::FRAGMENT };
		Game::Material material{ vertexShader, fragmentShader };
		Game::Mesh mesh{};

		::glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		while (window.IsRunning())
		{
			::glClear(GL_COLOR_BUFFER_BIT);

			::glUseProgram(material.NativeHandle());
			mesh.Bind();
			::glDrawArrays(GL_TRIANGLES, 0, 3);
			mesh.UnBind();

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
