#include "Utilities/Exception.h"
#include "UI/DebugUI.h"
#include "Logger.h"
#include "Texture.h"
#include "Sampler.h"
#include "Material.h"
#include "Renderer.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Mesh.h"
#include "Entity.h"
#include "Scene.h"
#include "ResourceLoader.h"

#include <iostream>
#include <print>
#include <numbers>
#include <cmath>
#include <type_traits>
#include <ranges>
#include <unordered_map>

int main(int argc, char** argv)
{
	Game::Logger::Info("Hello world");

	try
	{
		Game::Ensure(argc == 2, "game.exe <root_path>");

		Game::Window window{ 1280u, 720u };

		Game::ResourceLoader resourceLoader{ argv[1] };

		Game::Texture albedoTex{ resourceLoader.LoadBinary("textures/crate.png"), 500, 500 };
		Game::Sampler sampler{};
		Game::Texture specMap{ resourceLoader.LoadBinary("textures/crate_specular.png"), 500, 500 };

		const Game::Texture* textures[]{ &albedoTex, &specMap };
		const Game::Sampler* samplers[]{ &sampler, &sampler };
		const auto texSamp = std::views::zip(textures, samplers) | std::ranges::to<std::vector>();

		const Game::Shader vertexShader{ resourceLoader.LoadStr("shaders/basic.vert"), Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ resourceLoader.LoadStr("shaders/basic.frag"), Game::ShaderType::FRAGMENT };
		Game::Material material{ vertexShader, fragmentShader };
		const Game::Mesh mesh{};
		const Game::Renderer renderer{};
		
		std::vector<Game::Entity> entities{};

		for (int i = -10; i < 10; i++)
		{
			for (int j = -10; j < 10; j++)
			{
				entities.emplace_back(
					&mesh,
					&material,
					Game::vec3{ static_cast<float>(i) * 2.5f, -2.0f, static_cast<float>(j) * 2.5f },
					texSamp);
			}
		}

		Game::Scene scene{
			.entities = entities | std::views::transform([](const auto& e) { return &e; }) | std::ranges::to<std::vector>(),
			.ambient = {0.3f, 0.3f, 0.3f},
			.directionalLight = {.direction = {-1.0f, -1.0f, -1.0f}, .color = {0.5f, 0.5f, 0.5f}},
			.pointLight = {.position = {5.0f, 5.0f, 0.0f}, .color = {0.5f, 0.5f, 0.5f}}
		};

		Game::Camera camera{
			{0.0f, 0.0f, 5.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()),
			0.1f, 1000.0f
		};

		std::unordered_map<Game::Key, bool> keyState{};

		const Game::DebugUI debugUI{ window.GetNativeHandle(), scene };

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
					else if constexpr (std::same_as<T, Game::MouseEvent>)
					{
						static constexpr float sensitivity{ 0.002f };
						const float deltaX = arg.GetDeltaX() * sensitivity;
						const float deltaY = arg.GetDeltaY() * sensitivity;

						camera.AddYaw(deltaX);
						camera.AddPitch(-deltaY);
					}
					else if constexpr (std::same_as<T, Game::MouseButtonEvent>)
					{
						debugUI.AddMouseEvent(arg);
					}
				}, *event);
				event = window.PollEvent();
			}

			Game::vec3 walkDirection{ 0.0f, 0.0f, 0.0f };

			if (keyState[Game::Key::D])
			{
				walkDirection += camera.RightVector();
			}
			if (keyState[Game::Key::A])
			{
				walkDirection -= camera.RightVector();
			}
			if (keyState[Game::Key::W])
			{
				walkDirection += camera.GetDirection();
			}
			if (keyState[Game::Key::S])
			{
				walkDirection -= camera.GetDirection();
			}
			if (keyState[Game::Key::Q])
			{
				walkDirection -= Game::vec3{ 0.0f, 1.0f, 0.0f };
			}
			if (keyState[Game::Key::E])
			{
				walkDirection += Game::vec3{ 0.0f, 1.0f, 0.0f };
			}
			
			const float speed = 0.4f;
			camera.Translate(Game::vec3::Normalize(walkDirection) * speed);

			static float t = 0.0f;
			t += 0.01f;
			scene.pointLight.position.x = std::sin(t) * 10.0f;
			scene.pointLight.position.z = std::cos(t) * 10.0f;

			renderer.Render(camera, scene);
			debugUI.Render();
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
