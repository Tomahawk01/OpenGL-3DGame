#include "Game.h"

#include "Utilities/Exception.h"
#include "Utilities/Logger.h"
#include "UI/DebugUI.h"
#include "Core/Window.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Core/ResourceLoader.h"
#include "Core/ResourceCache.h"
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
#include "Game/Levels/LevelBravo.h"
#include "Game/Levels/LuaLevel.h"

#include <iostream>
#include <print>
#include <numbers>
#include <cmath>
#include <type_traits>
#include <ranges>
#include <unordered_map>
#include <random>

namespace Game {

	Game::Game()
		: m_Running{ true }
		, m_Levels{}
		, m_LevelNum{ 0u }
	{}

	void Game::Run(std::string_view resourceRoot)
	{
		MessageBus bus{};
		Subscriber::Subscribe(this, bus);

		Window window{ 1280u, 720u, 640u, 360u };
		Camera camera{
			{0.0f, 5.0f, 30.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			static_cast<float>(window.GetWidth()),
			static_cast<float>(window.GetHeight()),
			0.1f, 1000.0f
		};
		Player player{ bus, std::move(camera) };

		ResourceLoader resourceLoader{ resourceRoot };
		MeshLoader meshLoader{ resourceLoader };
		DefaultCache resourceCache{};

		const File tlvFile{ resourceLoader.Load("resources") };
		const TLVReader reader{ tlvFile.AsData() };

		const Sampler* sampler{ resourceCache.Insert<Sampler>("default") };
		resourceCache.Insert<Texture>("barrel_albedo", reader, "barrel_Albedo", sampler);
		resourceCache.Insert<Texture>("barrel_specular", reader, "barrel_Specular", sampler);
		resourceCache.Insert<Texture>("barrel_normal", reader, "barrel_Normal", sampler);

		Logger::Info("Textures loaded successfully");

		const File basicVertFile{ resourceLoader.Load("shaders/basic.vert") };
		const File barrelFragFile{ resourceLoader.Load("shaders/barrel.frag") };
		const File checkerboardFragFile{ resourceLoader.Load("shaders/checkerboard.frag") };

		const Shader vertexShader{ basicVertFile.AsString(), ShaderType::VERTEX };
		const Shader barrelFragmentShader{ barrelFragFile.AsString(), ShaderType::FRAGMENT };
		const Shader checkerboardShader{ checkerboardFragFile.AsString(), ShaderType::FRAGMENT };
		resourceCache.Insert<Mesh>("barrel", reader, "Barrel");
		resourceCache.Insert<Material>("barrel", vertexShader, barrelFragmentShader);
		resourceCache.Insert<Material>("floor", vertexShader, checkerboardShader);

		resourceCache.Insert<Texture>("floor_albedo",
			TextureDescription{
				.width = 1u,
				.height = 1u,
				.format = TextureFormat::RGB,
				.usage = TextureUsage::SRGB,
				.data = { static_cast<std::byte>(0xff), static_cast<std::byte>(0xff), static_cast<std::byte>(0xff) }
			}, sampler);
		resourceCache.Insert<Mesh>("floor", meshLoader.Cube());

		const Renderer renderer{ resourceLoader, meshLoader, window.GetWidth(), window.GetHeight() };

		m_Levels.push_back(std::make_unique<LuaLevel>(resourceLoader, "levels/level_alpha.lua", resourceCache, reader));

		m_Levels[m_LevelNum]->Restart();

		float gamma = 2.2f;

		WireframeRenderer wireframeRenderer{};

		while (m_Running)
		{
			auto event = window.PollEvent();
			while (event && m_Running)
			{
				std::visit([&](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;

					if constexpr (std::same_as<T, StopEvent>)
					{
						m_Running = false;
					}
					else if constexpr (std::same_as<T, KeyEvent>)
					{
						if (arg.GetKey() == Key::ESC)
						{
							m_Running = false;
						}

						bus.PostKeyPress(arg);
					}
					else if constexpr (std::same_as<T, MouseEvent>)
					{
						bus.PostMouseMove(arg);
					}
				}, *event);
				event = window.PollEvent();
			}

			auto* level = m_Levels[m_LevelNum].get();

			player.Update();
			level->Update(player);

			for (const auto& entity : level->GetEntities())
			{
				wireframeRenderer.Draw(entity.boundingBox);
			}

			wireframeRenderer.Draw(player.GetCamera());

			level->GetScene().debugLines = DebugLines{ wireframeRenderer.yield() };

			renderer.Render(player.GetCamera(), level->GetScene(), gamma);

			window.Swap();
		}
	}

	void Game::HandleLevelComplete(std::string_view levelName)
	{
		Logger::Info("Level complete: {}", levelName);
		m_LevelNum = (m_LevelNum + 1) % m_Levels.size();

		m_Levels[m_LevelNum]->Restart();
	}

}
