#include "Utilities/Exception.h"
#include "Utilities/Logger.h"
#include "UI/DebugUI.h"
#include "Core/Window.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Core/ResourceLoader.h"
#include "Renderer/Texture.h"
#include "Renderer/Sampler.h"
#include "Renderer/Material.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/MeshLoader.h"
#include "Renderer/Camera.h"
#include "TLV/TLVReader.h"

#include "Game/StaticObjectTransformer.h"
#include "Game/CameraObjectTransformer.h"

#include <iostream>
#include <print>
#include <numbers>
#include <cmath>
#include <type_traits>
#include <ranges>
#include <unordered_map>
#include <random>

struct TransformedEntity
{
	Game::Entity entity;
	std::unique_ptr<Game::ObjectTransformer> transformer;
};

int main(int argc, char** argv)
{
	Game::Logger::Info("Starting Game...");

	try
	{
		Game::Ensure(argc == 2, "game.exe <root_path>");

		Game::Window window{ 1280u, 720u, 640u, 360u };

		Game::Camera camera{
			{0.0f, 8.0f, 40.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			static_cast<float>(window.GetWidth()),
			static_cast<float>(window.GetHeight()),
			0.1f, 1000.0f
		};

		Game::ResourceLoader resourceLoader{ argv[1] };
		Game::MeshLoader meshLoader{ resourceLoader };

		const Game::File tlvFile{ resourceLoader.Load("resources") };
		const Game::TLVReader reader{ tlvFile.AsData() };
		
		Game::Sampler sampler{};
		Game::Texture albedoTex{ reader, "barrel_Albedo" };
		Game::Texture specMap{ reader, "barrel_Specular" };
		Game::Texture normalMap{ reader, "barrel_Normal" };

		Game::Logger::Info("Textures loaded successfully");

		const Game::Texture* textures[]{ &albedoTex, &specMap, &normalMap };
		const Game::Sampler* samplers[]{ &sampler, &sampler, &sampler };
		const auto texSamp = std::views::zip(textures, samplers) | std::ranges::to<std::vector>();

		const Game::File basicVertFile{ resourceLoader.Load("shaders/basic.vert") };
		const Game::File basicFragFile{ resourceLoader.Load("shaders/basic.frag") };

		const Game::Shader vertexShader{ basicVertFile.AsString(), Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ basicFragFile.AsString(), Game::ShaderType::FRAGMENT };
		Game::Material material{ vertexShader, fragmentShader };
		const Game::Mesh mesh{ reader, "Barrel" };

		const Game::Renderer renderer{ resourceLoader, meshLoader, window.GetWidth(), window.GetHeight() };

		std::vector<TransformedEntity> entities{};
		entities.emplace_back(
			Game::Entity{ &mesh, &material, {}, {1.0f}, texSamp },
			std::make_unique<Game::StaticObjectTransformer>(Game::vec3{})
		);
		entities.emplace_back(
			Game::Entity{ &mesh, &material, {5.0f, 0.0f, 0.0f}, {1.0f}, texSamp },
			std::make_unique<Game::CameraObjectTransformer>(Game::vec3{5.0f, 0.0f, 0.0f}, camera)
		);

		Game::Scene scene{
			.entities = entities | std::views::transform([](const auto& e) { return std::addressof(e.entity); }) | std::ranges::to<std::vector>(),
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
			},
			.debugLines = {}
		};

		Game::CubeMap skybox{
			reader,
			{{
				"right",
				"left",
				"top",
				"bottom",
				"front",
				"back"
			}}
		};
		Game::Sampler skyboxSampler{};

		std::unordered_map<Game::Key, bool> keyState{};

		float gamma = 2.2f;

		bool showDebug = false;
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

			walkDirection.y = 0.0f;
			
			const float speed = 0.5f;
			const Game::vec3 velocity = Game::vec3::Normalize(walkDirection) * speed;
			camera.Translate(velocity);

			for (auto& [entity, transformer] : entities)
			{
				transformer->Update();
				entity.SetPosition(transformer->Position());
			}

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
