#include "Game.h"

#include "Utilities/Exception.h"
#include "Utilities/Logger.h"
#include "UI/DebugUI.h"
#include "Core/Window.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "Core/ResourceLoader.h"
#include "Core/ResourceCache.h"
#include "Math/FrustumPlane.h"
#include "Physics/BoxShape.h"
#include "Physics/TransformedShape.h"
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

#include "Game/Player.h"

#include <iostream>
#include <print>
#include <numbers>
#include <cmath>
#include <type_traits>
#include <ranges>
#include <unordered_map>
#include <random>

namespace {

	bool IntersectsFrustum(const Game::TransformedShape& boundingBox, const std::array<Game::FrustumPlane, 6u>& planes)
	{
		Game::Expect(boundingBox.GetShape()->GetType() == Game::ShapeType::BOX, "This is not a box");

		const auto* boxShape = static_cast<const Game::BoxShape*>(boundingBox.GetShape());

		const auto position = boundingBox.GetTransform().Position;
		const auto min = position - boxShape->GetDimensions();
		const auto max = position + boxShape->GetDimensions();

		for (const auto& plane : planes)
		{
			Game::vec3 positiveVertex = min;
			if (plane.normal.x >= 0)
				positiveVertex.x = max.x;
			if (plane.normal.y >= 0)
				positiveVertex.y = max.y;
			if (plane.normal.z >= 0)
				positiveVertex.z = max.z;

			if (Game::vec3::Dot(plane.normal, positiveVertex) + plane.distance < 0.0f)
				return false;
		}

		return true;
	}

	Game::Camera CreateCamera(const Game::Window& window)
	{
		return {
			{0.0f, 5.0f, 30.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			std::numbers::pi_v<float> / 4.0f,
			static_cast<float>(window.GetWidth()),
			static_cast<float>(window.GetHeight()),
			0.1f, 1000.0f
		};
	}

}

namespace Game {

	Game::Game()
		: m_Running{ true }
		, m_LevelNames{
			"levels/level_alpha.lua",
			"levels/level_bravo.lua",
			"levels/level_charlie.lua",
			"levels/level_delta.lua"
		}
		, m_Level{}
		, m_LevelNum{ 0u }
		, m_Bus{}
		, m_Window{ 1280u, 720u, 640u, 360u }
		, m_Player{ m_Bus, CreateCamera(m_Window) }
	{
		Subscriber::Subscribe(this, m_Bus);
	}

	void Game::Run(std::string_view resourceRoot)
	{
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

		const Renderer renderer{ resourceLoader, meshLoader, m_Window.GetWidth(), m_Window.GetHeight() };

		float gamma = 2.2f;

		WireframeRenderer wireframeRenderer{};

		auto currentLevel = m_LevelNum;

		m_Level = std::make_unique<LuaLevel>(resourceLoader, m_LevelNames[m_LevelNum], resourceCache, reader, m_Player, m_Bus);

		while (m_Running)
		{
			if (currentLevel != m_LevelNum)
			{
				m_Player.Restart();
				m_Level.reset(nullptr);
				m_Level = std::make_unique<LuaLevel>(resourceLoader, m_LevelNames[m_LevelNum], resourceCache, reader, m_Player, m_Bus);
				currentLevel = m_LevelNum;
			}

			Expect(m_Level, "Level cannot be nullptr");

			auto event = m_Window.PollEvent();
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

						m_Bus.PostKeyPress(arg);
					}
					else if constexpr (std::same_as<T, MouseEvent>)
					{
						m_Bus.PostMouseMove(arg);
					}
				}, *event);
				event = m_Window.PollEvent();
			}

			m_Player.Update();
			m_Level->Update(m_Player);

			for (auto& entity : m_Level->GetScene().entities)
			{
				entity->SetVisibility(IntersectsFrustum(entity->GetBoundingBox(), m_Player.GetCamera().FrustumPlanes()));
			}

			wireframeRenderer.Draw(m_Player.GetCamera());

			renderer.Render(m_Player.GetCamera(), m_Level->GetScene(), gamma);

			m_Window.Swap();
		}
	}

	void Game::HandleLevelComplete(std::string_view levelName)
	{
		Logger::Info("Level complete: {}", levelName);
		m_LevelNum = (m_LevelNum + 1) % m_LevelNames.size();
	}

}
