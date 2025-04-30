#include "Utilities/Exception.h"
#include "Utilities/Logger.h"
#include "UI/DebugUI.h"
#include "Core/Window.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Core/ResourceLoader.h"
#include "Math/AABB.h"
#include "Math/FrustumPlane.h"
#include "Renderer/Texture.h"
#include "Renderer/Sampler.h"
#include "Renderer/Material.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/MeshLoader.h"
#include "Renderer/Camera.h"
#include "Renderer/WireframeRenderer.h"
#include "TLV/TLVReader.h"

#include "Game/Chain.h"
#include "Game/Player.h"
#include "Game/Levels/LevelAlpha.h"

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
	Game::Logger::Info("Starting Game...");

	try
	{
		Game::Ensure(argc == 2, "game.exe <root_path>");

		Game::MessageBus bus{};
		Game::Window window{ 1280u, 720u, 640u, 360u };
		Game::Camera camera{
			{0.0f, 5.0f, 30.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			static_cast<float>(window.GetWidth()),
			static_cast<float>(window.GetHeight()),
			0.1f, 1000.0f
		};
		Game::Player player{ bus, std::move(camera) };

		Game::ResourceLoader resourceLoader{ argv[1] };
		Game::MeshLoader meshLoader{ resourceLoader };

		const Game::File tlvFile{ resourceLoader.Load("resources") };
		const Game::TLVReader reader{ tlvFile.AsData() };
		
		Game::Sampler sampler{};
		Game::Texture albedoTex{ reader, "barrel_Albedo", &sampler };
		Game::Texture specMap{ reader, "barrel_Specular", &sampler };
		Game::Texture normalMap{ reader, "barrel_Normal", &sampler };

		Game::Logger::Info("Textures loaded successfully");

		const Game::Texture* textures[]{ &albedoTex, &specMap, &normalMap };

		const Game::File basicVertFile{ resourceLoader.Load("shaders/basic.vert") };
		const Game::File barrelFragFile{ resourceLoader.Load("shaders/barrel.frag") };
		const Game::File checkerboardFragFile{ resourceLoader.Load("shaders/checkerboard.frag") };

		const Game::Shader vertexShader{ basicVertFile.AsString(), Game::ShaderType::VERTEX };
		const Game::Shader barrelFragmentShader{ barrelFragFile.AsString(), Game::ShaderType::FRAGMENT };
		const Game::Shader checkerboardShader{ checkerboardFragFile.AsString(), Game::ShaderType::FRAGMENT };
		Game::Material barrelMaterial{ vertexShader, barrelFragmentShader };
		Game::Material checkerboardMaterial{ vertexShader, checkerboardShader };
		const Game::Mesh mesh{ reader, "Barrel" };

		const Game::Texture floorTexture{
			Game::TextureDescription{
				.width = 1u,
				.height = 1u,
				.format = Game::TextureFormat::RGB,
				.usage = Game::TextureUsage::SRGB,
				.data = { static_cast<std::byte>(0xff), static_cast<std::byte>(0xff), static_cast<std::byte>(0xff) }
		}, &sampler };
		const Game::Texture* floorTextures[] = { &floorTexture, &floorTexture };
		const Game::Mesh floorMesh{ meshLoader.Cube() };

		const Game::Renderer renderer{ resourceLoader, meshLoader, window.GetWidth(), window.GetHeight() };
		
		Game::LevelAlpha level{
			&floorMesh, &checkerboardMaterial, floorTextures,
			&mesh, &barrelMaterial, textures,
			reader, player
		};

		float gamma = 2.2f;

		bool showDebug = false;
		const Game::DebugUI debugUI{ window.GetNativeHandle(), level.GetScene(), player.GetCamera(), gamma};

		Game::WireframeRenderer wireframeRenderer{};

		bool running = true;
		while (running)
		{
			auto event = window.PollEvent();
			while (event && running)
			{
				std::visit([&](auto&& arg)
				{
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

						bus.PostKeyPress(arg);
						
						/*if (arg.GetKey() == Game::Key::F1 && arg.GetState() == Game::KeyState::UP)
						{
							showDebug = !showDebug;
						}*/
					}
					else if constexpr (std::same_as<T, Game::MouseEvent>)
					{
						bus.PostMouseMove(arg);
					}
					else if constexpr (std::same_as<T, Game::MouseButtonEvent>)
					{
						debugUI.AddMouseEvent(arg);
					}
				}, *event);
				event = window.PollEvent();
			}

			player.Update();
			level.Update(player);

			if (level.Complete())
			{
				running = false;
			}

			wireframeRenderer.Draw(player.GetCamera());

			level.GetScene().debugLines = Game::DebugLines{ wireframeRenderer.yield() };

			renderer.Render(player.GetCamera(), level.GetScene(), gamma);

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
