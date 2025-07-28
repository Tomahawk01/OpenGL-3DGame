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

		//resourceCache.Insert<Texture>("warehouse_albedo", reader, "warehouse_Albedo", sampler);
		//resourceCache.Insert<Texture>("warehouse_normal", reader, "warehouse_Normal", sampler);

		Logger::Info("Textures loaded successfully");

		const TextFile basicVertFile = GetFile(reader, "basic.vert");
		const TextFile barrelFragFile = GetFile(reader, "barrel.frag");
		const TextFile checkerboardFragFile = GetFile(reader, "checkerboard.frag");

		const Shader vertexShader{ basicVertFile.Data, ShaderType::VERTEX };
		const Shader barrelFragmentShader{ barrelFragFile.Data, ShaderType::FRAGMENT };
		const Shader checkerboardShader{ checkerboardFragFile.Data, ShaderType::FRAGMENT };
		resourceCache.Insert<Mesh>("barrel", reader, "Barrel");
		resourceCache.Insert<Mesh>("sponza_bricks_05", reader, "sponza_05_sponza_05_bricks");
		resourceCache.Insert<Mesh>("sponza_bricks_06", reader, "sponza_06_sponza_06_bricks");
		resourceCache.Insert<Mesh>("sponza_arch_07", reader, "sponza_07_sponza_07_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_08", reader, "sponza_08_sponza_08_ceiling");
		resourceCache.Insert<Mesh>("sponza_column_a_09", reader, "sponza_09_sponza_09_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_10", reader, "sponza_10_sponza_10_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_11", reader, "sponza_11_sponza_11_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_12", reader, "sponza_12_sponza_12_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_13", reader, "sponza_13_sponza_13_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_14", reader, "sponza_14_sponza_14_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_15", reader, "sponza_15_sponza_15_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_16", reader, "sponza_16_sponza_16_column_a");
		resourceCache.Insert<Mesh>("sponza_arch_17", reader, "sponza_17_sponza_17_arch");
		resourceCache.Insert<Mesh>("sponza_floor_18", reader, "sponza_18_sponza_18_floor");
		resourceCache.Insert<Mesh>("sponza_ceiling_19", reader, "sponza_19_sponza_19_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_20", reader, "sponza_20_sponza_20_arch");
		resourceCache.Insert<Mesh>("sponza_arch_21", reader, "sponza_21_sponza_21_arch");
		resourceCache.Insert<Mesh>("sponza_column_c_22", reader, "sponza_22_sponza_22_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_23", reader, "sponza_23_sponza_23_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_24", reader, "sponza_24_sponza_24_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_25", reader, "sponza_25_sponza_25_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_26", reader, "sponza_26_sponza_26_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_27", reader, "sponza_27_sponza_27_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_28", reader, "sponza_28_sponza_28_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_29", reader, "sponza_29_sponza_29_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_30", reader, "sponza_30_sponza_30_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_31", reader, "sponza_31_sponza_31_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_32", reader, "sponza_32_sponza_32_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_33", reader, "sponza_33_sponza_33_column_c");
		resourceCache.Insert<Mesh>("sponza_bricks_34", reader, "sponza_34_sponza_34_bricks");
		resourceCache.Insert<Mesh>("sponza_ceiling_35", reader, "sponza_35_sponza_35_ceiling");
		resourceCache.Insert<Mesh>("sponza_bricks_36", reader, "sponza_36_sponza_36_bricks");
		resourceCache.Insert<Mesh>("sponza_arch_37", reader, "sponza_37_sponza_37_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_38", reader, "sponza_38_sponza_38_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_39", reader, "sponza_39_sponza_39_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_40", reader, "sponza_40_sponza_40_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_41", reader, "sponza_41_sponza_41_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_42", reader, "sponza_42_sponza_42_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_43", reader, "sponza_43_sponza_43_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_44", reader, "sponza_44_sponza_44_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_45", reader, "sponza_45_sponza_45_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_46", reader, "sponza_46_sponza_46_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_47", reader, "sponza_47_sponza_47_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_48", reader, "sponza_48_sponza_48_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_49", reader, "sponza_49_sponza_49_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_50", reader, "sponza_50_sponza_50_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_51", reader, "sponza_51_sponza_51_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_52", reader, "sponza_52_sponza_52_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_53", reader, "sponza_53_sponza_53_arch");
		resourceCache.Insert<Mesh>("sponza_ceiling_54", reader, "sponza_54_sponza_54_ceiling");
		resourceCache.Insert<Mesh>("sponza_arch_55", reader, "sponza_55_sponza_55_arch");
		resourceCache.Insert<Mesh>("sponza_arch_56", reader, "sponza_56_sponza_56_arch");
		resourceCache.Insert<Mesh>("sponza_arch_57", reader, "sponza_57_sponza_57_arch");
		resourceCache.Insert<Mesh>("sponza_arch_58", reader, "sponza_58_sponza_58_arch");
		resourceCache.Insert<Mesh>("sponza_arch_59", reader, "sponza_59_sponza_59_arch");
		resourceCache.Insert<Mesh>("sponza_arch_60", reader, "sponza_60_sponza_60_arch");
		resourceCache.Insert<Mesh>("sponza_arch_61", reader, "sponza_61_sponza_61_arch");
		resourceCache.Insert<Mesh>("sponza_arch_62", reader, "sponza_62_sponza_62_arch");
		resourceCache.Insert<Mesh>("sponza_arch_63", reader, "sponza_63_sponza_63_arch");
		resourceCache.Insert<Mesh>("sponza_arch_64", reader, "sponza_64_sponza_64_arch");
		resourceCache.Insert<Mesh>("sponza_arch_65", reader, "sponza_65_sponza_65_arch");
		resourceCache.Insert<Mesh>("sponza_bricks_66", reader, "sponza_66_sponza_66_bricks");
		resourceCache.Insert<Mesh>("sponza_arch_67", reader, "sponza_67_sponza_67_arch");
		resourceCache.Insert<Mesh>("sponza_bricks_68", reader, "sponza_68_sponza_68_bricks");
		resourceCache.Insert<Mesh>("sponza_bricks_69", reader, "sponza_69_sponza_69_bricks");
		resourceCache.Insert<Mesh>("sponza_floor_117", reader, "sponza_117_sponza_117_floor");
		resourceCache.Insert<Mesh>("sponza_column_a_118", reader, "sponza_118_sponza_118_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_119", reader, "sponza_119_sponza_119_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_120", reader, "sponza_120_sponza_120_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_121", reader, "sponza_121_sponza_121_column_a");
		resourceCache.Insert<Mesh>("sponza_bricks_379", reader, "sponza_379_sponza_379_bricks");
		resourceCache.Insert<Mesh>("sponza_roof_380", reader, "sponza_380_sponza_380_roof");
		resourceCache.Insert<Mesh>("sponza_roof_381", reader, "sponza_381_sponza_381_roof");

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
