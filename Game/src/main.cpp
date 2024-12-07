#include "Utilities/Exception.h"
#include "Logger.h"
#include "Material.h"
#include "Renderer.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Mesh.h"
#include "Entity.h"
#include "Scene.h"

#include <iostream>
#include <print>
#include <numbers>

namespace {

	static constexpr const char* vertex_shader_src = R"(
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 vertex_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
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
	Game::mat4 m1{ {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f} };
	Game::mat4 m2{ {1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f} };

	m1 *= m2;
	std::println("{}", m1);

	try
	{
		Game::Window window{ 800u, 600u };

		const Game::Shader vertexShader{ vertex_shader_src, Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ fragment_shader_src, Game::ShaderType::FRAGMENT };
		Game::Material material{ vertexShader, fragmentShader };
		const Game::Mesh mesh{};
		const Game::Renderer renderer{};
		const Game::Entity entity1{ &mesh, &material, {.x = 0.0f, .y = -1.0f, .z = 0.0f} };
		const Game::Entity entity2{&mesh, &material, {.x = 0.0f, .y = 2.0f, .z = 0.0f} };
		Game::Scene scene{ {&entity1, &entity2} };

		const Game::Camera camera{
			{3.0f, 0.0f, 5.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			800.0f, 600.0f,
			0.1f, 100.0f
		};

		while (window.IsRunning())
		{
			renderer.Render(camera, scene);
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
