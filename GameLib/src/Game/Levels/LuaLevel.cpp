#include "LuaLevel.h"

#include "Utilities/Error.h"
#include "Utilities/StringMap.h"
#include "Renderer/TextFactory.h"
#include "Renderer/UI/Label.h"
#include "Scripting/ScriptRunner.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/BoxShape.h"
#include "Physics/TransformedShape.h"
#include "TLV/Utilities.h"

#include <ranges>
#include <algorithm>

using namespace std::literals;

namespace Game {

	LuaLevel::LuaLevel(const ScriptLoader& loader, DefaultCache& resourceCache, const TLVReader& reader, const Player& player, MessageBus& bus)
		: m_PS{}
		, m_Script{ loader.Load() }
		, m_Entities{}
		, m_LevelEntities{}
		, m_Skybox{ reader, {{ "right", "left", "top", "bottom", "front", "back" }} }
		, m_ResourceCache{ resourceCache }
		, m_Bus{ bus }
		, m_BarrelInfo{}
		, m_Shapes{}
		, m_AutoSub{ m_Bus, {MessageType::ENTITY_INTERSECT}, this }
	{
		Logger::Info("Loading level: {}", loader.GetName());

		const Texture* barrelTextures[]{
			resourceCache.Get<Texture>("barrel_albedo"),
			resourceCache.Get<Texture>("barrel_specular"),
			resourceCache.Get<Texture>("barrel_normal")
		};

		const ScriptRunner runner{ m_Script };
		runner.Execute("init_level", player.GetPosition());

		const auto barrelCount = runner.Execute<int64_t>("barrel_count");
		for (int64_t i = 0; i < barrelCount; i++)
		{
			auto* shape = m_PS.CreateShape<BoxShape>(vec3{ 0.6f, 1.0f, 0.6f });
			m_Shapes.push_back(shape);

			const auto info = runner.Execute<vec3, vec3, float, int64_t, int64_t>("barrel_info", i + 1ll);
			m_Entities.emplace_back(
				resourceCache.Get<Mesh>("barrel"),
				resourceCache.Get<Material>("barrel"),
				std::get<0>(info), vec3{ 1.0f },
				barrelTextures,
				TransformedShape{ shape, {std::get<0>(info), {1.0f}, {}} },
				static_cast<uint32_t>(std::get<3>(info)),
				static_cast<uint32_t>(std::get<4>(info))
			);
		}

		const auto ambientVec = m_Script.HasFunction("get_ambient") ? runner.Execute<vec3>("get_ambient") : vec3{ 0.4f };
		const auto [directionalLightDir, directionalLightColor] = m_Script.HasFunction("get_directional_light")
			? runner.Execute<vec3, vec3>("get_directional_light")
			: std::make_tuple(vec3{ -1.0f, -1.0f, 0.0f }, vec3{ 0.5f });

		const TextFactory textFactory{};
		static auto textTest = textFactory.Create("Hello world ❤️ 叶 💩", resourceCache.Get<Sampler>("ui"), 32u, Colors::Azure);

		m_Scene = Scene{
			.entities = m_Entities | std::views::transform([](auto& e) { return std::addressof(e); }) | std::ranges::to<std::vector>(),
			.ambient = {ambientVec.x, ambientVec.y, ambientVec.z},
			.directionalLight = {
				.direction = directionalLightDir,
				.color = {directionalLightColor.x, directionalLightColor.y, directionalLightColor.z}
			},
			.pointLights = {
				{.position = {5.0f, 5.0f, 0.0f},
				.color = {1.0f, 0.0f, 0.0f},
				.constAttenuation = 1.0f,
				.linearAttenuation = 0.07f,
				.quadAttenuation = 0.007f },

				{.position = {-5.0f, 5.0f, 0.0f},
				.color = {0.0f, 1.0f, 0.0f},
				.constAttenuation = 1.0f,
				.linearAttenuation = 0.07f,
				.quadAttenuation = 0.007f },

				{.position = {-5.0f, 5.0f, 0.0f},
				.color = {0.0f, 0.0f, 1.0f},
				.constAttenuation = 1.0f,
				.linearAttenuation = 0.07f,
				.quadAttenuation = 0.007f }
			},
			.debugLines = {},
			.skybox = &m_Skybox,
			.skyboxSampler = resourceCache.Get<Sampler>("sky_box"),
			.labels = {
				{&textTest, 0, 0}
			},
			.effects = {
				.hdr = true,
				.grayScale = false,
				.blur = false
			}
		};

		const std::vector levelEntityNames = {
			"sponza_bricks_05"sv,
			"sponza_bricks_06"sv,
			"sponza_arch_07"sv,
			"sponza_ceiling_08"sv,
			"sponza_column_a_09"sv,
			"sponza_column_a_10"sv,
			"sponza_column_a_11"sv,
			"sponza_column_a_12"sv,
			"sponza_column_a_13"sv,
			"sponza_column_a_14"sv,
			"sponza_column_a_15"sv,
			"sponza_column_a_16"sv,
			"sponza_arch_17"sv,
			"sponza_floor_18"sv,
			"sponza_ceiling_19"sv,
			"sponza_arch_20"sv,
			"sponza_arch_21"sv,
			"sponza_column_c_22"sv,
			"sponza_column_c_23"sv,
			"sponza_column_c_24"sv,
			"sponza_column_c_25"sv,
			"sponza_column_c_26"sv,
			"sponza_column_c_27"sv,
			"sponza_column_c_28"sv,
			"sponza_column_c_29"sv,
			"sponza_column_c_30"sv,
			"sponza_column_c_31"sv,
			"sponza_column_c_32"sv,
			"sponza_column_c_33"sv,
			"sponza_bricks_34"sv,
			"sponza_ceiling_35"sv,
			"sponza_bricks_36"sv,
			"sponza_arch_37"sv,
			"sponza_ceiling_38"sv,
			"sponza_arch_39"sv,
			"sponza_ceiling_40"sv,
			"sponza_arch_41"sv,
			"sponza_ceiling_42"sv,
			"sponza_arch_43"sv,
			"sponza_ceiling_44"sv,
			"sponza_arch_45"sv,
			"sponza_ceiling_46"sv,
			"sponza_arch_47"sv,
			"sponza_ceiling_48"sv,
			"sponza_arch_49"sv,
			"sponza_ceiling_50"sv,
			"sponza_arch_51"sv,
			"sponza_ceiling_52"sv,
			"sponza_arch_53"sv,
			"sponza_ceiling_54"sv,
			"sponza_arch_55"sv,
			"sponza_arch_56"sv,
			"sponza_arch_57"sv,
			"sponza_arch_58"sv,
			"sponza_arch_59"sv,
			"sponza_arch_60"sv,
			"sponza_arch_61"sv,
			"sponza_arch_62"sv,
			"sponza_arch_63"sv,
			"sponza_arch_64"sv,
			"sponza_arch_65"sv,
			"sponza_bricks_66"sv,
			"sponza_arch_67"sv,
			"sponza_bricks_68"sv,
			"sponza_bricks_69"sv,
			"sponza_details_70"sv,
			"sponza_details_71"sv,
			"sponza_details_72"sv,
			"sponza_details_73"sv,
			"sponza_details_74"sv,
			"sponza_bricks_75"sv,
			"sponza_column_c_76"sv,
			"sponza_column_c_77"sv,
			"sponza_column_c_78"sv,
			"sponza_column_c_79"sv,
			"sponza_column_c_80"sv,
			"sponza_column_c_81"sv,
			"sponza_column_c_82"sv,
			"sponza_column_c_83"sv,
			"sponza_column_c_84"sv,
			"sponza_column_c_85"sv,
			"sponza_column_c_86"sv,
			"sponza_column_c_87"sv,
			"sponza_column_c_88"sv,
			"sponza_column_c_89"sv,
			"sponza_column_c_90"sv,
			"sponza_column_c_91"sv,
			"sponza_column_c_92"sv,
			"sponza_column_c_93"sv,
			"sponza_column_c_94"sv,
			"sponza_column_c_95"sv,
			"sponza_column_c_96"sv,
			"sponza_column_c_97"sv,
			"sponza_column_c_98"sv,
			"sponza_column_c_99"sv,
			"sponza_column_c_100"sv,
			"sponza_column_c_101"sv,
			"sponza_column_c_102"sv,
			"sponza_column_c_103"sv,
			"sponza_column_c_104"sv,
			"sponza_column_c_105"sv,
			"sponza_column_c_106"sv,
			"sponza_column_c_107"sv,
			"sponza_column_c_108"sv,
			"sponza_column_c_109"sv,
			"sponza_column_c_110"sv,
			"sponza_column_c_111"sv,
			"sponza_column_c_112"sv,
			"sponza_column_c_113"sv,
			"sponza_column_c_114"sv,
			"sponza_column_c_115"sv,
			"sponza_bricks_116"sv,
			"sponza_floor_117"sv,
			"sponza_column_a_118"sv,
			"sponza_column_a_119"sv,
			"sponza_column_a_120"sv,
			"sponza_column_a_121"sv,
			"sponza_arch_122"sv,
			"sponza_arch_123"sv,
			"sponza_arch_124"sv,
			"sponza_column_b_125"sv,
			"sponza_column_b_126"sv,
			"sponza_column_b_127"sv,
			"sponza_column_b_128"sv,
			"sponza_column_b_129"sv,
			"sponza_column_b_130"sv,
			"sponza_column_b_131"sv,
			"sponza_column_b_132"sv,
			"sponza_column_b_133"sv,
			"sponza_column_b_134"sv,
			"sponza_column_b_135"sv,
			"sponza_column_b_136"sv,
			"sponza_column_b_137"sv,
			"sponza_column_b_138"sv,
			"sponza_column_b_139"sv,
			"sponza_column_b_140"sv,
			"sponza_column_b_141"sv,
			"sponza_column_b_142"sv,
			"sponza_column_b_143"sv,
			"sponza_column_b_144"sv,
			"sponza_column_b_145"sv,
			"sponza_column_b_146"sv,
			"sponza_column_b_147"sv,
			"sponza_column_b_148"sv,
			"sponza_column_b_149"sv,
			"sponza_column_b_150"sv,
			"sponza_column_b_151"sv,
			"sponza_column_b_152"sv,
			"sponza_column_b_153"sv,
			"sponza_column_b_154"sv,
			"sponza_column_b_155"sv,
			"sponza_column_b_156"sv,
			"sponza_column_b_157"sv,
			"sponza_column_b_158"sv,
			"sponza_column_b_159"sv,
			"sponza_column_b_160"sv,
			"sponza_column_b_161"sv,
			"sponza_column_b_162"sv,
			"sponza_column_b_163"sv,
			"sponza_column_b_164"sv,
			"sponza_column_b_165"sv,
			"sponza_column_b_166"sv,
			"sponza_column_b_167"sv,
			"sponza_column_b_168"sv,
			"sponza_column_b_169"sv,
			"sponza_column_b_170"sv,
			"sponza_column_b_171"sv,
			"sponza_column_b_172"sv,
			"sponza_column_b_173"sv,
			"sponza_column_b_174"sv,
			"sponza_column_b_175"sv,
			"sponza_column_b_176"sv,
			"sponza_column_b_177"sv,
			"sponza_column_b_178"sv,
			"sponza_column_b_179"sv,
			"sponza_column_b_180"sv,
			"sponza_column_b_181"sv,
			"sponza_column_b_182"sv,
			"sponza_column_b_183"sv,
			"sponza_column_b_184"sv,
			"sponza_column_b_185"sv,
			"sponza_column_b_186"sv,
			"sponza_column_b_187"sv,
			"sponza_column_b_188"sv,
			"sponza_column_b_189"sv,
			"sponza_column_b_190"sv,
			"sponza_column_b_191"sv,
			"sponza_column_b_192"sv,
			"sponza_column_b_193"sv,
			"sponza_column_b_194"sv,
			"sponza_column_b_195"sv,
			"sponza_column_b_196"sv,
			"sponza_column_b_197"sv,
			"sponza_column_b_198"sv,
			"sponza_column_b_199"sv,
			"sponza_column_b_200"sv,
			"sponza_column_b_201"sv,
			"sponza_column_b_202"sv,
			"sponza_column_b_203"sv,
			"sponza_column_b_204"sv,
			"sponza_column_b_205"sv,
			"sponza_column_b_206"sv,
			"sponza_column_b_207"sv,
			"sponza_column_b_208"sv,
			"sponza_column_b_209"sv,
			"sponza_column_b_210"sv,
			"sponza_column_b_211"sv,
			"sponza_column_b_212"sv,
			"sponza_column_b_213"sv,
			"sponza_column_b_214"sv,
			"sponza_column_b_215"sv,
			"sponza_column_b_216"sv,
			"sponza_column_b_217"sv,
			"sponza_column_b_218"sv,
			"sponza_column_b_219"sv,
			"sponza_column_b_220"sv,
			"sponza_column_b_221"sv,
			"sponza_column_b_222"sv,
			"sponza_column_b_223"sv,
			"sponza_column_b_224"sv,
			"sponza_column_b_225"sv,
			"sponza_column_b_226"sv,
			"sponza_column_b_227"sv,
			"sponza_column_b_228"sv,
			"sponza_column_b_229"sv,
			"sponza_column_b_230"sv,
			"sponza_column_b_231"sv,
			"sponza_column_b_232"sv,
			"sponza_column_b_233"sv,
			"sponza_column_b_234"sv,
			"sponza_column_b_235"sv,
			"sponza_column_b_236"sv,
			"sponza_column_b_237"sv,
			"sponza_column_b_238"sv,
			"sponza_column_b_239"sv,
			"sponza_column_b_240"sv,
			"sponza_column_b_241"sv,
			"sponza_column_b_242"sv,
			"sponza_column_b_243"sv,
			"sponza_column_b_244"sv,
			"sponza_column_b_245"sv,
			"sponza_column_b_246"sv,
			"sponza_column_b_247"sv,
			"sponza_column_b_248"sv,
			"sponza_column_b_249"sv,
			"sponza_column_b_250"sv,
			"sponza_column_b_251"sv,
			"sponza_column_b_252"sv,
			"sponza_column_b_253"sv,
			"sponza_column_b_254"sv,
			"sponza_column_b_255"sv,
			"sponza_column_b_256"sv,
			"sponza_bricks_258"sv,
			"sponza_flagpole_259"sv,
			"sponza_flagpole_260"sv,
			"sponza_flagpole_261"sv,
			"sponza_flagpole_262"sv,
			"sponza_flagpole_263"sv,
			"sponza_flagpole_264"sv,
			"sponza_flagpole_265"sv,
			"sponza_flagpole_266"sv,
			"sponza_flagpole_267"sv,
			"sponza_flagpole_268"sv,
			"sponza_flagpole_269"sv,
			"sponza_flagpole_270"sv,
			"sponza_flagpole_271"sv,
			"sponza_flagpole_272"sv,
			"sponza_flagpole_273"sv,
			"sponza_flagpole_274"sv,
			"sponza_vase_373"sv,
			"sponza_vase_374"sv,
			"sponza_vase_375"sv,
			"sponza_vase_376"sv,
			"sponza_Material__25_377"sv,
			"sponza_Material__298_377"sv,
			"sponza_Material__25_378"sv,
			"sponza_Material__298_378"sv,
			"sponza_bricks_379"sv,
			"sponza_roof_380"sv,
			"sponza_roof_381"sv,
			"sponza_bricks_382"sv
		};

		const StringMap<std::tuple<std::string_view, std::string_view>> textureLookup{
			{ "sponza_bricks_05", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_06", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_34", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_36", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_66", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_68", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_69", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_75", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_116", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_258", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_379", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },
			{ "sponza_bricks_382", std::make_tuple("sponza_bricks_a_albedo"sv, "sponza_bricks_a_normal"sv) },

			{ "sponza_arch_07", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_17", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_20", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_21", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_37", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_39", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_41", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_43", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_45", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_47", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_49", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_51", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_53", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_55", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_56", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_57", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_58", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_59", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_60", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_61", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_62", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_63", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_64", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_65", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_67", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_122", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_123", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },
			{ "sponza_arch_124", std::make_tuple("sponza_arch_albedo"sv, "sponza_arch_normal"sv) },

			{ "sponza_column_a_09", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_10", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_11", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_12", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_13", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_14", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_15", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_16", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_118", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_119", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_120", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },
			{ "sponza_column_a_121", std::make_tuple("sponza_column_a_albedo"sv, "sponza_column_a_normal"sv) },

			{ "sponza_column_b_125", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_126", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_127", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_128", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_129", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_130", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_131", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_132", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_133", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_134", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_135", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_136", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_137", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_138", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_139", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_140", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_141", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_142", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_143", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_144", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_145", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_146", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_147", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_148", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_149", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_150", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_151", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_152", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_153", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_154", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_155", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_156", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_157", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_158", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_159", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_160", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_161", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_162", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_163", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_164", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_165", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_166", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_167", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_168", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_169", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_170", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_171", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_172", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_173", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_174", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_175", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_176", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_177", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_178", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_179", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_180", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_181", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_182", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_183", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_184", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_185", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_186", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_187", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_188", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_189", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_190", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_191", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_192", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_193", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_194", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_195", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_196", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_197", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_198", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_199", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_200", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_201", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_202", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_203", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_204", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_205", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_206", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_207", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_208", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_209", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_210", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_211", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_212", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_213", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_214", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_215", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_216", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_217", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_218", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_219", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_220", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_221", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_222", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_223", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_224", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_225", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_226", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_227", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_228", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_229", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_230", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_231", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_232", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_233", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_234", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_235", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_236", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_237", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_238", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_239", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_240", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_241", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_242", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_243", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_244", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_245", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_246", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_247", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_248", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_249", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_250", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_251", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_252", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_253", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_254", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_255", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },
			{ "sponza_column_b_256", std::make_tuple("sponza_column_b_albedo"sv, "sponza_column_b_normal"sv) },

			{ "sponza_column_c_22", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_23", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_24", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_25", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_26", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_27", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_28", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_29", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_30", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_31", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_32", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_33", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },

			{ "sponza_column_c_76", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_77", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_78", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_79", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_80", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_81", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_82", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_83", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_84", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_85", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_86", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_87", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_88", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_89", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_90", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_91", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_92", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_93", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_94", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_95", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_96", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_97", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_98", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_99", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_100", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_101", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_102", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_103", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_104", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_105", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_106", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_107", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_108", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_109", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_110", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_111", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_112", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_113", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_114", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },
			{ "sponza_column_c_115", std::make_tuple("sponza_column_c_albedo"sv, "sponza_column_c_normal"sv) },

			{ "sponza_floor_18", std::make_tuple("sponza_floor_a_albedo"sv, "sponza_floor_a_normal"sv) },
			{ "sponza_floor_117", std::make_tuple("sponza_floor_a_albedo"sv, "sponza_floor_a_normal"sv) },

			{ "sponza_ceiling_08", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_19", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_35", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_38", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_40", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_42", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_44", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_46", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_48", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_50", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_52", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },
			{ "sponza_ceiling_54", std::make_tuple("sponza_ceiling_a_albedo"sv, "sponza_ceiling_a_normal"sv) },

			{ "sponza_details_70", std::make_tuple("sponza_details_albedo"sv, "sponza_details_normal"sv) },
			{ "sponza_details_71", std::make_tuple("sponza_details_albedo"sv, "sponza_details_normal"sv) },
			{ "sponza_details_72", std::make_tuple("sponza_details_albedo"sv, "sponza_details_normal"sv) },
			{ "sponza_details_73", std::make_tuple("sponza_details_albedo"sv, "sponza_details_normal"sv) },
			{ "sponza_details_74", std::make_tuple("sponza_details_albedo"sv, "sponza_details_normal"sv) },

			{ "sponza_flagpole_259", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_260", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_261", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_262", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_263", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_264", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_265", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_266", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_267", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_268", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_269", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_270", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_271", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_272", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_273", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },
			{ "sponza_flagpole_274", std::make_tuple("sponza_flagpole_albedo"sv, "sponza_flagpole_normal"sv) },

			{ "sponza_vase_373", std::make_tuple("sponza_vase_albedo"sv, "sponza_vase_normal"sv) },
			{ "sponza_vase_374", std::make_tuple("sponza_vase_albedo"sv, "sponza_vase_normal"sv) },
			{ "sponza_vase_375", std::make_tuple("sponza_vase_albedo"sv, "sponza_vase_normal"sv) },
			{ "sponza_vase_376", std::make_tuple("sponza_vase_albedo"sv, "sponza_vase_normal"sv) },

			{ "sponza_Material__25_377", std::make_tuple("sponza_lion_albedo"sv, "sponza_lion_normal"sv) },
			{ "sponza_Material__25_378", std::make_tuple("sponza_lion_albedo"sv, "sponza_lion_normal"sv) },
			{ "sponza_Material__298_377", std::make_tuple("sponza_background_albedo"sv, "sponza_background_normal"sv) },
			{ "sponza_Material__298_378", std::make_tuple("sponza_background_albedo"sv, "sponza_background_normal"sv) },

			{ "sponza_roof_380", std::make_tuple("sponza_roof_albedo"sv, "sponza_roof_normal"sv) },
			{ "sponza_roof_381", std::make_tuple("sponza_roof_albedo"sv, "sponza_roof_normal"sv) },
		};

		m_LevelEntities = levelEntityNames | std::views::transform(
			[&](const auto e)
			{
				auto* material = m_ResourceCache.Get<Material>("floor");
				auto* albedo = m_ResourceCache.Get<Texture>("checkerboard");
				auto* normal = m_ResourceCache.Get<Texture>("sponza_floor_a_normal");

				if (const auto lookup = textureLookup.find(e); lookup != std::ranges::cend(textureLookup))
				{
					const auto& textures = lookup->second;

					material = m_ResourceCache.Get<Material>("basic");
					albedo = m_ResourceCache.Get<Texture>(std::get<0>(textures));
					normal = m_ResourceCache.Get<Texture>(std::get<1>(textures));
				}

				return Entity{ 
					resourceCache.Get<Mesh>(e),
					material,
					{ 0.0f, -2.0f, 0.0f },
					{ 0.09f },
					std::vector<const Texture*>{ albedo, normal },
					{ m_PS.CreateShape<BoxShape>(vec3{50.0f, 0.5f, 50.0f}), {{0.0f, -2.0f, 0.0f}, {1.0f}, {}} },
					0u,
					0u
				};
			}
		) | std::ranges::to<std::vector>();

		for (auto& ent : m_LevelEntities)
		{
			m_Scene.entities.push_back(&ent);
		}

		Restart();
	}

	void LuaLevel::Update(const Player& player)
	{
		const ScriptRunner runner{ m_Script };

		for (const auto& [index, entity] : std::views::enumerate(m_Entities))
		{
			runner.Execute("set_barrel_visibility", index + 1ll, entity.IsVisible());
		}

		auto origPositions = m_Entities |
							 std::views::transform([](const auto& e) { return std::make_tuple(false, e.GetPosition()); }) |
							 std::ranges::to<std::vector>();

		runner.Execute("update_level", player.GetPosition());

		const auto max = std::ranges::size(m_Entities);

		for (const auto& [index, entity] : std::views::enumerate(m_Entities))
		{
			const auto& [position, color, tintAmount, collisionLayer, collisionMask] = runner.Execute<vec3, vec3, float, int64_t, int64_t>("barrel_info", index + 1ll);

			entity.SetPosition(position);

			m_BarrelInfo[std::addressof(entity)] = {
				.tintColor = { color.x, color.y, color.z },
				.tintAmount = tintAmount
			};
		}

		auto combs = std::views::iota(0ull, max) |
					 std::views::transform(
					 [max](auto x)
					 {
							 return std::views::iota(x + 1ull, max) |
								 std::views::transform([x](auto y) { return std::make_pair(x, y); });
					 }) | std::views::join;

		for (const auto [i, j] : combs)
		{
			const Entity& ent1 = m_Entities[i];
			const Entity& ent2 = m_Entities[j];

			if ((ent1.GetCollisionMask() & ent2.GetCollisionLayer()) && (ent2.GetCollisionMask() & ent1.GetCollisionLayer()))
			{
				const TransformedShape& transformShape1{ m_Shapes[i], {ent1.GetPosition(), {1.0f}, {}} };
				const TransformedShape& transformShape2{ m_Shapes[j], {ent2.GetPosition(), {1.0f}, {}} };

				if (transformShape1.Intersects(transformShape2))
				{
					std::get<0>(origPositions[i]) = true;
					std::get<0>(origPositions[j]) = true;
					m_Bus.PostEntityIntersect(std::addressof(ent1), std::addressof(ent2));
				}
			}
		}

		const auto levelState = static_cast<LevelState>(runner.Execute<int64_t>("level_state"));
		switch (levelState)
		{
			case LevelState::COMPLETE:
			{
				m_Bus.PostLevelComplete("lua_level");
			} break;
			case LevelState::LOST:
			{
				Restart();
				m_Bus.PostRestartLevel();
			} break;
			default:
			{
				for (const auto& [index, orig] : std::views::enumerate(origPositions))
				{
					if (const auto& [revert, origPosition] = orig; revert)
					{
						m_Entities[index].SetPosition(origPosition);
						runner.Execute("set_barrel_position", index + 1ll, origPosition);
					}
				}
			} break;
		}

		const auto ambientVec = m_Script.HasFunction("get_ambient") ? runner.Execute<vec3>("get_ambient") : vec3{ 0.4f };
		const auto [directionalLightDir, directionalLightColor] = m_Script.HasFunction("get_directional_light")
			? runner.Execute<vec3, vec3>("get_directional_light")
			: std::make_tuple(vec3{ -1.0f, -1.0f, 0.0f }, vec3{ 0.5f });
		m_Scene.ambient = { ambientVec.x, ambientVec.y, ambientVec.z };
		m_Scene.directionalLight = {
			.direction = directionalLightDir,
			.color = {directionalLightColor.x, directionalLightColor.y, directionalLightColor.z}
		};

		m_PS.Debug_Renderer().Clear();
		for (auto i = 0u; i < max; i++)
		{
			const TransformedShape& transformShape{ m_Shapes[i], {m_Entities[i].GetPosition(), {1.0f}, {}} };
			transformShape.Draw(m_PS.Debug_Renderer());
		}

		m_Scene.debugLines = m_PS.Debug_Renderer().GetLines();
	}

	void LuaLevel::Restart()
	{
		const ScriptRunner runner{ m_Script };
		runner.Execute("restart_level");

		m_ResourceCache.Get<Material>("barrel")->SetUniformCallback(
			[this](const auto* mat, const auto* ent)
			{
				if (const auto info = m_BarrelInfo.find(ent); info != std::ranges::cend(m_BarrelInfo))
				{
					mat->SetUniform("tint_color", info->second.tintColor);
					mat->SetUniform("tint_amount", info->second.tintAmount);
				}
			}
		);

		for (const auto& [index, entity] : std::views::enumerate(m_Entities))
		{
			const auto& [position, color, tintAmount, collisionLayer, collisionMask] = runner.Execute<vec3, vec3, float, int64_t, int64_t>("barrel_info", index + 1ll);

			entity.SetPosition(position);
		}
	}

	void LuaLevel::HandleEntityIntersect(const Entity* a, const Entity* b)
	{
		const Entity* begin = m_Entities.data();
		const auto indexA = static_cast<int64_t>(a - begin);
		const auto indexB = static_cast<int64_t>(b - begin);

		Expect(indexA >= 0 && indexA < static_cast<int64_t>(m_Entities.size()), "indexA {} out of range", indexA);
		Expect(indexB >= 0 && indexB < static_cast<int64_t>(m_Entities.size()), "indexB {} out of range", indexB);

		const ScriptRunner runner{ m_Script };

		runner.Execute("handle_entity_intersect", indexA + 1ll, indexB + 1ll);
	}

	std::span<const Entity> LuaLevel::GetEntities() const
	{
		return m_Entities;
	}

}
