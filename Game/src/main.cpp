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
#include "MeshLoader.h"

#include <iostream>
#include <print>
#include <numbers>
#include <cmath>
#include <type_traits>
#include <ranges>
#include <unordered_map>
#include <random>

int main(int argc, char** argv)
{
	Game::Logger::Info("Hello world");

	try
	{
		Game::Ensure(argc == 2, "game.exe <root_path>");

		Game::Window window{ 1280u, 720u };

		Game::ResourceLoader resourceLoader{ argv[1] };
		Game::MeshLoader meshLoader{ resourceLoader };

		Game::Sampler sampler{};
		Game::Texture albedoTex{ Game::TextureUsage::SRGB, resourceLoader.LoadBinary("textures/falcon_Albedo.png"), 4096, 4096 };
		Game::Texture specMap{ Game::TextureUsage::DATA, resourceLoader.LoadBinary("textures/falcon_Specular.png"), 4096, 4096 };
		Game::Texture normalMap{ Game::TextureUsage::DATA, resourceLoader.LoadBinary("textures/falcon_Normal.png"), 4096, 4096 };

		const Game::Texture* textures[]{ &albedoTex, &specMap, &normalMap };
		const Game::Sampler* samplers[]{ &sampler, &sampler, &sampler };
		const auto texSamp = std::views::zip(textures, samplers) | std::ranges::to<std::vector>();

		const Game::Shader vertexShader{ resourceLoader.LoadStr("shaders/basic.vert"), Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ resourceLoader.LoadStr("shaders/basic.frag"), Game::ShaderType::FRAGMENT };
		Game::Material material{ vertexShader, fragmentShader };
		const Game::Mesh mesh{ meshLoader.Load("models/falcon.obj", "Plane_Plane.001")};
		const Game::Renderer renderer{ resourceLoader, meshLoader, window.GetWidth(), window.GetHeight() };

		std::vector<Game::Entity> entities{};
			
		for (int i = -10; i < 10; i++)
		{
			for (int j = -10; j < 10; j++)
			{
				entities.emplace_back(
					&mesh,
					&material,
					Game::vec3{ static_cast<float>(i) * 15.0f, 0.0f, static_cast<float>(j) * 15.0f },
					Game::vec3{ 1.0f },
					texSamp);
			}
		}

		Game::Scene scene{
			.entities = entities | std::views::transform([](const auto& e) { return &e; }) | std::ranges::to<std::vector>(),
			.ambient = {0.3f, 0.3f, 0.3f},
			.directionalLight = {.direction = {-1.0f, -1.0f, -1.0f}, .color = {0.5f, 0.5f, 0.5f}},
			.pointLights = {
				{.position = {5.0f, 5.0f, 0.0f},
				.color = {0.0f, 1.0f, 0.0f},
				.constAttenuation = 1.0f,
				.linearAttenuation = 0.07f,
				.quadAttenuation = 0.007f },

				{.position = {-5.0f, 5.0f, 0.0f},
				.color = {1.0f, 0.0f, 0.0f},
				.constAttenuation = 1.0f,
				.linearAttenuation = 0.07f,
				.quadAttenuation = 0.007f }
			}
		};

		Game::Camera camera{
			{0.0f, 0.0f, 5.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()),
			0.1f, 1000.0f
		};

		Game::CubeMap skybox{
			{resourceLoader.LoadBinary("textures/right.jpg"),
			 resourceLoader.LoadBinary("textures/left.jpg"),
			 resourceLoader.LoadBinary("textures/top.jpg"),
			 resourceLoader.LoadBinary("textures/bottom.jpg"),
			 resourceLoader.LoadBinary("textures/front.jpg"),
			 resourceLoader.LoadBinary("textures/back.jpg")},
			2048u, 2048u
		};
		Game::Sampler skyboxSampler{};

		std::unordered_map<Game::Key, bool> keyState{};

		float gamma = 2.2f;

		bool showDebug = true;
		const Game::DebugUI debugUI{ window.GetNativeHandle(), scene, camera, gamma };

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

						if (arg.GetKey() == Game::Key::F1 && arg.GetState() == Game::KeyState::UP)
						{
							showDebug = !showDebug;
						}
					}
					else if constexpr (std::same_as<T, Game::MouseEvent>)
					{
						if (!showDebug)
						{
							static constexpr float sensitivity{ 0.002f };
							const float deltaX = arg.GetDeltaX() * sensitivity;
							const float deltaY = arg.GetDeltaY() * sensitivity;

							camera.AddYaw(deltaX);
							camera.AddPitch(-deltaY);
						}
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

			renderer.Render(camera, scene, skybox, skyboxSampler, gamma);
			if (showDebug)
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
