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
#include <type_traits>
#include <ranges>
#include <unordered_map>

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
	Game::Logger::Info("Hello world");

	try
	{
		Game::Window window{ 800u, 600u };

		const Game::Shader vertexShader{ vertex_shader_src, Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ fragment_shader_src, Game::ShaderType::FRAGMENT };
		Game::Material material{ vertexShader, fragmentShader };
		const Game::Mesh mesh{};
		const Game::Renderer renderer{};
		
		std::vector<Game::Entity> entities{};
		for (int i = -10; i < 10; i++)
		{
			for (int j = -10; j < 10; j++)
			{
				entities.emplace_back(&mesh, &material, Game::vec3{ static_cast<float>(i) * 2.5f, -2.0f, static_cast<float>(j) * 2.5f });
			}
		}

		const Game::Scene scene{ entities | std::views::transform([](const auto& e) { return &e; }) | std::ranges::to<std::vector>() };

		Game::Camera camera{
			{0.0f, 0.0f, 5.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			800.0f, 600.0f,
			0.1f, 1000.0f
		};

		std::unordered_map<Game::Key, bool> keyState{};

		bool running = true;
		while (running)
		{
			auto event = window.PollEvent();
			while (event && running)
			{
				std::visit([&](auto&& arg) {
					using T = std::decay_t<decltype(arg)>;

					if constexpr (std::same_as<T, Game::StopEvent>)
					{
						running = false;
					}
					else if constexpr (std::same_as<T, Game::KeyEvent>)
					{
						if (arg.GetKey() == Game::Key::ESC)
						{
							running = false;
						}
						
						keyState[arg.GetKey()] = arg.GetState() == Game::KeyState::DOWN;
					}
				}, * event);
				event = window.PollEvent();
			}

			const Game::vec3 velocity{
				(keyState[Game::Key::D] ? 1.0f : 0.0f) + (keyState[Game::Key::A] ? -1.0f : 0.0f),
				(keyState[Game::Key::E] ? 1.0f : 0.0f) + (keyState[Game::Key::Q] ? -1.0f : 0.0f),
				(keyState[Game::Key::S] ? 1.0f : 0.0f) + (keyState[Game::Key::W] ? -1.0f : 0.0f)
			};

			camera.Translate(Game::vec3::Normalize(velocity));

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
