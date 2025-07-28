#include "LuaLevel.h"

#include "Utilities/Error.h"
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
		, m_SkyboxSampler{}
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
			.skyboxSampler = &m_SkyboxSampler
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
			"sponza_floor_117"sv,
			"sponza_column_a_118"sv,
			"sponza_column_a_119"sv,
			"sponza_column_a_120"sv,
			"sponza_column_a_121"sv,
			"sponza_bricks_379"sv,
			"sponza_roof_380"sv,
			"sponza_roof_381"sv,
		};

		m_LevelEntities = levelEntityNames | std::views::transform(
			[&](const auto e)
			{
				return Entity{ 
					resourceCache.Get<Mesh>(e),
					resourceCache.Get<Material>("floor"),
					{ 0.0f, -2.0f, 0.0f },
					{ 0.05f },
					std::vector<const Texture*>{resourceCache.Get<Texture>("floor_albedo"), resourceCache.Get<Texture>("floor_albedo")},
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
