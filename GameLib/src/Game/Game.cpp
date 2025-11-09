#include "Game.h"

#include "Utilities/Decompress.h"
#include "Core/Window.h"

#include "Game/Routines/InputRoutine.h"
#include "Game/Routines/LevelRoutine.h"
#include "Game/Routines/RenderRoutine.h"
#include "Game/Routines/MainMenuRoutine.h"
#include "Game/Routines/SoundRoutine.h"
#include "Game/Routines/PhysicsRoutine.h"
#include "config.h"

#include <ShellScalingApi.h>

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

	Game::WindowMode _WindowMode()
	{
		return Game::Config::IsFullscreen() ? Game::WindowMode::FULLSCREEN : Game::WindowMode::WINDOWED;
	}

}

namespace Game {

	Game::Game(const std::vector<std::string_view>& args)
		: m_Running{ true }
		, m_Bus{}
		, m_Window{ _WindowMode(), 1920u, 1080u, GetArg("-x", args), GetArg("-y", args) }
	{
		Ensure(SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK, "Failed to set dpi aware");
	}

	void Game::Run(std::string_view resourceRoot)
	{
		ResourceLoader resourceLoader{ resourceRoot };
		MeshLoader meshLoader{ resourceLoader };
		DefaultCache resourceCache{};

		resourceCache.Insert<Sampler>("sky_box", Sampler{ FilterType::NEAREST, FilterType::NEAREST });
		resourceCache.Insert<Sampler>("ui", Sampler{ FilterType::NEAREST, FilterType::NEAREST });
		const auto* mipmap = resourceCache.Insert<Sampler>("mipmap", Sampler{ FilterType::LINEAR_MIPMAP, FilterType::LINEAR, 16.0f });

		const auto tlvFile = [&resourceLoader]
		{
			const File compressedTLVFile{ resourceLoader.Load("resources") };
			return Decompress(compressedTLVFile.AsData());
		}();
		const TLVReader reader{ tlvFile };

		resourceCache.Insert<Texture>("barrel_albedo", reader, "barrel_Albedo", mipmap);
		resourceCache.Insert<Texture>("barrel_specular", reader, "barrel_Specular", mipmap);
		resourceCache.Insert<Texture>("barrel_normal", reader, "barrel_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_bricks_a_albedo", reader, "spnza_bricks_a_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_bricks_a_normal", reader, "spnza_bricks_a_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_arch_albedo", reader, "sponza_arch_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_arch_normal", reader, "sponza_arch_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_column_a_albedo", reader, "sponza_column_a_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_column_a_normal", reader, "sponza_column_a_Normal", mipmap);
		resourceCache.Insert<Texture>("sponza_column_b_albedo", reader, "sponza_column_b_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_column_b_normal", reader, "sponza_column_b_Normal", mipmap);
		resourceCache.Insert<Texture>("sponza_column_c_albedo", reader, "sponza_column_c_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_column_c_normal", reader, "sponza_column_c_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_floor_a_albedo", reader, "sponza_floor_a_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_floor_a_normal", reader, "sponza_floor_a_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_ceiling_a_albedo", reader, "sponza_ceiling_a_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_ceiling_a_normal", reader, "sponza_ceiling_a_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_roof_albedo", reader, "sponza_roof_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_roof_normal", reader, "sponza_roof_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_details_albedo", reader, "sponza_details_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_details_normal", reader, "sponza_details_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_flagpole_albedo", reader, "sponza_flagpole_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_flagpole_normal", reader, "sponza_flagpole_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_vase_albedo", reader, "vase_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_vase_normal", reader, "vase_Normal", mipmap);

		resourceCache.Insert<Texture>("sponza_background_albedo", reader, "background_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_background_normal", reader, "background_Normal", mipmap);
		resourceCache.Insert<Texture>("sponza_lion_albedo", reader, "lion_Albedo", mipmap);
		resourceCache.Insert<Texture>("sponza_lion_normal", reader, "lion_Normal", mipmap);

		Logger::Info("Textures loaded successfully");

		const TextFile basicVertFile = GetFile(reader, "basic.vert");
		const TextFile basicFragFile = GetFile(reader, "basic.frag");
		const TextFile barrelFragFile = GetFile(reader, "barrel.frag");
		const TextFile checkerboardFragFile = GetFile(reader, "checkerboard.frag");

		const Shader vertexShader{ basicVertFile.Data, ShaderType::VERTEX };
		const Shader basicFragmentShader{ basicFragFile.Data, ShaderType::FRAGMENT };
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
		resourceCache.Insert<Mesh>("sponza_details_70", reader, "sponza_70_sponza_70_details");
		resourceCache.Insert<Mesh>("sponza_details_71", reader, "sponza_71_sponza_71_details");
		resourceCache.Insert<Mesh>("sponza_details_72", reader, "sponza_72_sponza_72_details");
		resourceCache.Insert<Mesh>("sponza_details_73", reader, "sponza_73_sponza_73_details");
		resourceCache.Insert<Mesh>("sponza_details_74", reader, "sponza_74_sponza_74_details");
		resourceCache.Insert<Mesh>("sponza_bricks_75", reader, "sponza_75_sponza_75_bricks");
		resourceCache.Insert<Mesh>("sponza_column_c_76", reader, "sponza_76_sponza_76_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_77", reader, "sponza_77_sponza_77_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_78", reader, "sponza_78_sponza_78_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_79", reader, "sponza_79_sponza_79_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_80", reader, "sponza_80_sponza_80_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_81", reader, "sponza_81_sponza_81_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_82", reader, "sponza_82_sponza_82_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_83", reader, "sponza_83_sponza_83_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_84", reader, "sponza_84_sponza_84_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_85", reader, "sponza_85_sponza_85_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_86", reader, "sponza_86_sponza_86_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_87", reader, "sponza_87_sponza_87_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_88", reader, "sponza_88_sponza_88_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_89", reader, "sponza_89_sponza_89_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_90", reader, "sponza_90_sponza_90_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_91", reader, "sponza_91_sponza_91_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_92", reader, "sponza_92_sponza_92_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_93", reader, "sponza_93_sponza_93_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_94", reader, "sponza_94_sponza_94_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_95", reader, "sponza_95_sponza_95_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_96", reader, "sponza_96_sponza_96_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_97", reader, "sponza_97_sponza_97_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_98", reader, "sponza_98_sponza_98_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_99", reader, "sponza_99_sponza_99_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_100", reader, "sponza_100_sponza_100_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_101", reader, "sponza_101_sponza_101_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_102", reader, "sponza_102_sponza_102_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_103", reader, "sponza_103_sponza_103_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_104", reader, "sponza_104_sponza_104_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_105", reader, "sponza_105_sponza_105_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_106", reader, "sponza_106_sponza_106_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_107", reader, "sponza_107_sponza_107_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_108", reader, "sponza_108_sponza_108_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_109", reader, "sponza_109_sponza_109_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_110", reader, "sponza_110_sponza_110_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_111", reader, "sponza_111_sponza_111_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_112", reader, "sponza_112_sponza_112_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_113", reader, "sponza_113_sponza_113_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_114", reader, "sponza_114_sponza_114_column_c");
		resourceCache.Insert<Mesh>("sponza_column_c_115", reader, "sponza_115_sponza_115_column_c");
		resourceCache.Insert<Mesh>("sponza_bricks_116", reader, "sponza_116_sponza_116_bricks");
		resourceCache.Insert<Mesh>("sponza_floor_117", reader, "sponza_117_sponza_117_floor");
		resourceCache.Insert<Mesh>("sponza_column_a_118", reader, "sponza_118_sponza_118_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_119", reader, "sponza_119_sponza_119_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_120", reader, "sponza_120_sponza_120_column_a");
		resourceCache.Insert<Mesh>("sponza_column_a_121", reader, "sponza_121_sponza_121_column_a");
		resourceCache.Insert<Mesh>("sponza_arch_122", reader, "sponza_122_sponza_122_arch");
		resourceCache.Insert<Mesh>("sponza_arch_123", reader, "sponza_123_sponza_123_arch");
		resourceCache.Insert<Mesh>("sponza_arch_124", reader, "sponza_124_sponza_124_arch");
		resourceCache.Insert<Mesh>("sponza_column_b_125", reader, "sponza_125_sponza_125_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_126", reader, "sponza_126_sponza_126_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_127", reader, "sponza_127_sponza_127_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_128", reader, "sponza_128_sponza_128_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_129", reader, "sponza_129_sponza_129_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_130", reader, "sponza_130_sponza_130_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_131", reader, "sponza_131_sponza_131_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_132", reader, "sponza_132_sponza_132_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_133", reader, "sponza_133_sponza_133_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_134", reader, "sponza_134_sponza_134_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_135", reader, "sponza_135_sponza_135_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_136", reader, "sponza_136_sponza_136_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_137", reader, "sponza_137_sponza_137_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_138", reader, "sponza_138_sponza_138_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_139", reader, "sponza_139_sponza_139_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_140", reader, "sponza_140_sponza_140_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_141", reader, "sponza_141_sponza_141_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_142", reader, "sponza_142_sponza_142_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_143", reader, "sponza_143_sponza_143_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_144", reader, "sponza_144_sponza_144_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_145", reader, "sponza_145_sponza_145_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_146", reader, "sponza_146_sponza_146_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_147", reader, "sponza_147_sponza_147_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_148", reader, "sponza_148_sponza_148_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_149", reader, "sponza_149_sponza_149_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_150", reader, "sponza_150_sponza_150_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_151", reader, "sponza_151_sponza_151_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_152", reader, "sponza_152_sponza_152_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_153", reader, "sponza_153_sponza_153_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_154", reader, "sponza_154_sponza_154_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_155", reader, "sponza_155_sponza_155_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_156", reader, "sponza_156_sponza_156_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_157", reader, "sponza_157_sponza_157_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_158", reader, "sponza_158_sponza_158_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_159", reader, "sponza_159_sponza_159_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_160", reader, "sponza_160_sponza_160_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_161", reader, "sponza_161_sponza_161_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_162", reader, "sponza_162_sponza_162_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_163", reader, "sponza_163_sponza_163_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_164", reader, "sponza_164_sponza_164_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_165", reader, "sponza_165_sponza_165_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_166", reader, "sponza_166_sponza_166_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_167", reader, "sponza_167_sponza_167_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_168", reader, "sponza_168_sponza_168_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_169", reader, "sponza_169_sponza_169_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_170", reader, "sponza_170_sponza_170_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_171", reader, "sponza_171_sponza_171_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_172", reader, "sponza_172_sponza_172_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_173", reader, "sponza_173_sponza_173_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_174", reader, "sponza_174_sponza_174_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_175", reader, "sponza_175_sponza_175_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_176", reader, "sponza_176_sponza_176_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_177", reader, "sponza_177_sponza_177_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_178", reader, "sponza_178_sponza_178_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_179", reader, "sponza_179_sponza_179_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_180", reader, "sponza_180_sponza_180_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_181", reader, "sponza_181_sponza_181_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_182", reader, "sponza_182_sponza_182_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_183", reader, "sponza_183_sponza_183_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_184", reader, "sponza_184_sponza_184_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_185", reader, "sponza_185_sponza_185_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_186", reader, "sponza_186_sponza_186_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_187", reader, "sponza_187_sponza_187_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_188", reader, "sponza_188_sponza_188_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_189", reader, "sponza_189_sponza_189_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_190", reader, "sponza_190_sponza_190_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_191", reader, "sponza_191_sponza_191_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_192", reader, "sponza_192_sponza_192_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_193", reader, "sponza_193_sponza_193_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_194", reader, "sponza_194_sponza_194_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_195", reader, "sponza_195_sponza_195_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_196", reader, "sponza_196_sponza_196_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_197", reader, "sponza_197_sponza_197_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_198", reader, "sponza_198_sponza_198_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_199", reader, "sponza_199_sponza_199_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_200", reader, "sponza_200_sponza_200_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_201", reader, "sponza_201_sponza_201_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_202", reader, "sponza_202_sponza_202_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_203", reader, "sponza_203_sponza_203_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_204", reader, "sponza_204_sponza_204_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_205", reader, "sponza_205_sponza_205_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_206", reader, "sponza_206_sponza_206_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_207", reader, "sponza_207_sponza_207_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_208", reader, "sponza_208_sponza_208_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_209", reader, "sponza_209_sponza_209_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_210", reader, "sponza_210_sponza_210_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_211", reader, "sponza_211_sponza_211_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_212", reader, "sponza_212_sponza_212_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_213", reader, "sponza_213_sponza_213_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_214", reader, "sponza_214_sponza_214_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_215", reader, "sponza_215_sponza_215_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_216", reader, "sponza_216_sponza_216_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_217", reader, "sponza_217_sponza_217_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_218", reader, "sponza_218_sponza_218_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_219", reader, "sponza_219_sponza_219_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_220", reader, "sponza_220_sponza_220_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_221", reader, "sponza_221_sponza_221_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_222", reader, "sponza_222_sponza_222_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_223", reader, "sponza_223_sponza_223_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_224", reader, "sponza_224_sponza_224_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_225", reader, "sponza_225_sponza_225_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_226", reader, "sponza_226_sponza_226_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_227", reader, "sponza_227_sponza_227_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_228", reader, "sponza_228_sponza_228_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_229", reader, "sponza_229_sponza_229_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_230", reader, "sponza_230_sponza_230_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_231", reader, "sponza_231_sponza_231_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_232", reader, "sponza_232_sponza_232_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_233", reader, "sponza_233_sponza_233_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_234", reader, "sponza_234_sponza_234_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_235", reader, "sponza_235_sponza_235_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_236", reader, "sponza_236_sponza_236_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_237", reader, "sponza_237_sponza_237_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_238", reader, "sponza_238_sponza_238_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_239", reader, "sponza_239_sponza_239_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_240", reader, "sponza_240_sponza_240_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_241", reader, "sponza_241_sponza_241_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_242", reader, "sponza_242_sponza_242_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_243", reader, "sponza_243_sponza_243_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_244", reader, "sponza_244_sponza_244_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_245", reader, "sponza_245_sponza_245_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_246", reader, "sponza_246_sponza_246_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_247", reader, "sponza_247_sponza_247_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_248", reader, "sponza_248_sponza_248_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_249", reader, "sponza_249_sponza_249_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_250", reader, "sponza_250_sponza_250_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_251", reader, "sponza_251_sponza_251_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_252", reader, "sponza_252_sponza_252_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_253", reader, "sponza_253_sponza_253_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_254", reader, "sponza_254_sponza_254_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_255", reader, "sponza_255_sponza_255_column_b");
		resourceCache.Insert<Mesh>("sponza_column_b_256", reader, "sponza_256_sponza_256_column_b");
		resourceCache.Insert<Mesh>("sponza_Material__47_257", reader, "sponza_257_sponza_257_Material__47");
		resourceCache.Insert<Mesh>("sponza_bricks_258", reader, "sponza_258_sponza_258_bricks");
		resourceCache.Insert<Mesh>("sponza_flagpole_259", reader, "sponza_259_sponza_259_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_260", reader, "sponza_260_sponza_260_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_261", reader, "sponza_261_sponza_261_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_262", reader, "sponza_262_sponza_262_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_263", reader, "sponza_263_sponza_263_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_264", reader, "sponza_264_sponza_264_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_265", reader, "sponza_265_sponza_265_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_266", reader, "sponza_266_sponza_266_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_267", reader, "sponza_267_sponza_267_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_268", reader, "sponza_268_sponza_268_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_269", reader, "sponza_269_sponza_269_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_270", reader, "sponza_270_sponza_270_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_271", reader, "sponza_271_sponza_271_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_272", reader, "sponza_272_sponza_272_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_273", reader, "sponza_273_sponza_273_flagpole");
		resourceCache.Insert<Mesh>("sponza_flagpole_274", reader, "sponza_274_sponza_274_flagpole");
		/*275-281 leaf*/
		resourceCache.Insert<Mesh>("sponza_fabric_e_282", reader, "sponza_282_sponza_282_fabric_e");
		resourceCache.Insert<Mesh>("sponza_fabric_d_283", reader, "sponza_283_sponza_283_fabric_d");
		resourceCache.Insert<Mesh>("sponza_fabric_a_284", reader, "sponza_284_sponza_284_fabric_a");
		resourceCache.Insert<Mesh>("sponza_fabric_e_285", reader, "sponza_285_sponza_285_fabric_e");
		resourceCache.Insert<Mesh>("sponza_fabric_d_286", reader, "sponza_286_sponza_286_fabric_d");
		resourceCache.Insert<Mesh>("sponza_fabric_e_287", reader, "sponza_287_sponza_287_fabric_e");
		resourceCache.Insert<Mesh>("sponza_fabric_a_288", reader, "sponza_288_sponza_288_fabric_a");
		resourceCache.Insert<Mesh>("sponza_fabric_d_289", reader, "sponza_289_sponza_289_fabric_d");
		/*290-329 rgb-fabric*/
		/*330-365 hanging vases*/
		/*366-372 floor vases*/
		resourceCache.Insert<Mesh>("sponza_vase_373", reader, "sponza_373_sponza_373_vase");
		resourceCache.Insert<Mesh>("sponza_vase_374", reader, "sponza_374_sponza_374_vase");
		resourceCache.Insert<Mesh>("sponza_vase_375", reader, "sponza_375_sponza_375_vase");
		resourceCache.Insert<Mesh>("sponza_vase_376", reader, "sponza_376_sponza_376_vase");
		resourceCache.Insert<Mesh>("sponza_Material__25_377", reader, "sponza_377_sponza_377_Material__25");
		resourceCache.Insert<Mesh>("sponza_Material__298_377", reader, "sponza_377_sponza_377_Material__298");
		resourceCache.Insert<Mesh>("sponza_Material__25_378", reader, "sponza_378_sponza_378_Material__25");
		resourceCache.Insert<Mesh>("sponza_Material__298_378", reader, "sponza_378_sponza_378_Material__298");
		resourceCache.Insert<Mesh>("sponza_bricks_379", reader, "sponza_379_sponza_379_bricks");
		resourceCache.Insert<Mesh>("sponza_roof_380", reader, "sponza_380_sponza_380_roof");
		resourceCache.Insert<Mesh>("sponza_roof_381", reader, "sponza_381_sponza_381_roof");
		resourceCache.Insert<Mesh>("sponza_bricks_382", reader, "sponza_382_sponza_382_bricks");

		resourceCache.Insert<Material>("barrel", vertexShader, barrelFragmentShader);
		resourceCache.Insert<Material>("floor", vertexShader, checkerboardShader);
		resourceCache.Insert<Material>("basic", vertexShader, basicFragmentShader);

		resourceCache.Insert<Texture>(
			"checkerboard",
			TextureDescription{
				.width = 1u,
				.height = 1u,
				.format = TextureFormat::RGB,
				.usage = TextureUsage::SRGB,
				.data = { static_cast<std::byte>(0xff), static_cast<std::byte>(0xff), static_cast<std::byte>(0xff) }
			}, mipmap);
		resourceCache.Insert<Mesh>("floor", meshLoader.Cube());

		const auto mainThemeData = std::ranges::find_if(reader, [](const auto& entry) { return entry.IsSoundData("main_theme"); });
		Ensure(mainThemeData != std::ranges::cend(reader), "Could not find main theme");
		resourceCache.Insert<SoundData>("main_theme", (*mainThemeData).SoundDataValue());

		PhysicsSystem ps{};

		Scheduler scheduler{ m_Bus };

		InputRoutine inputRoutine{ m_Window, m_Bus, scheduler };
		SoundRoutine soundRoutine{ m_Bus, scheduler, resourceCache };
		LevelRoutine levelRoutine{ ps, m_Window, m_Bus, scheduler, resourceCache, reader };
		RenderRoutine renderRoutine{ m_Window, m_Bus, scheduler, meshLoader, reader };
		MainMenuRoutine mainMenuRoutine{ ps, m_Window, m_Bus, scheduler, resourceCache, reader };
		PhysicsRoutine physicsRoutine{ ps, m_Bus, scheduler };

		scheduler.Add(inputRoutine.CreateTask());
		scheduler.Add(physicsRoutine.CreateTask());
		scheduler.Add(soundRoutine.CreateTask());
		scheduler.Add(mainMenuRoutine.CreateTask());
		scheduler.Add(levelRoutine.CreateTask());
		scheduler.Add(renderRoutine.CreateTask());

		scheduler.Run();
	}

}
