#include "Game.h"

#include "Utilities/Exception.h"
#include "Utilities/Logger.h"
#include "Utilities/Decompress.h"
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
#include "TLV/Utilities.h"

#include "Game/Player.h"
#include "Game/Routines/InputRoutine.h"
#include "Game/Routines/LevelRoutine.h"
#include "Game/Routines/RenderRoutine.h"

#include <iostream>
#include <print>
#include <cmath>
#include <type_traits>
#include <ranges>
#include <unordered_map>
#include <random>

namespace {

	uint32_t GetArg(std::string_view argName, const std::vector<std::string_view>& args)
	{
		const auto arg = std::ranges::find(args, argName);
		if (arg == std::ranges::cend(args))
		{
			return 0u;
		}

		const auto index = static_cast<size_t>(std::ranges::distance(std::ranges::cbegin(args), arg));
		if (index + 1u >= args.size())
		{
			return 0u;
		}

		return std::stol(std::string{ args[index + 1u] });
	}

}

namespace Game {

	Game::Game(const std::vector<std::string_view>& args)
		: m_Running{ true }
		, m_Bus{}
		, m_Window{ 1280u, 720u, GetArg("-x", args), GetArg("-y", args) }
	{}

	void Game::Run(std::string_view resourceRoot)
	{
		ResourceLoader resourceLoader{ resourceRoot };
		MeshLoader meshLoader{ resourceLoader };
		DefaultCache resourceCache{};

		const Sampler* sampler{ resourceCache.Insert<Sampler>("default") };

		const auto tlvFile = [&resourceLoader]
		{
			const File compressedTLVFile{ resourceLoader.Load("resources") };
			return Decompress(compressedTLVFile.AsData());
		}();
		const TLVReader reader{ tlvFile };

		resourceCache.Insert<Texture>("barrel_albedo", reader, "barrel_Albedo", sampler);
		resourceCache.Insert<Texture>("barrel_specular", reader, "barrel_Specular", sampler);
		resourceCache.Insert<Texture>("barrel_normal", reader, "barrel_Normal", sampler);

		Logger::Info("Textures loaded successfully");

		const TextFile basicVertFile = GetFile(reader, "basic.vert");
		const TextFile barrelFragFile = GetFile(reader, "barrel.frag");
		const TextFile checkerboardFragFile = GetFile(reader, "checkerboard.frag");

		const Shader vertexShader{ basicVertFile.Data, ShaderType::VERTEX };
		const Shader barrelFragmentShader{ barrelFragFile.Data, ShaderType::FRAGMENT };
		const Shader checkerboardShader{ checkerboardFragFile.Data, ShaderType::FRAGMENT };
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

		Scheduler scheduler{};

		InputRoutine inputRoutine{ m_Window, m_Bus, scheduler };
		LevelRoutine levelRoutine{ m_Window, m_Bus, scheduler, resourceCache, reader };
		RenderRoutine renderRoutine{ levelRoutine, m_Window, m_Bus, scheduler, meshLoader, reader };

		scheduler.Add(inputRoutine.CreateTask());
		scheduler.Add(levelRoutine.CreateTask());
		scheduler.Add(renderRoutine.CreateTask());

		scheduler.Run();
	}

}
