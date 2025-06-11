#include "LuaLevel.h"

#include "Scripting/ScriptRunner.h"
#include "Physics/BoxShape.h"
#include "Physics/TransformedShape.h"

#include <ranges>
#include <algorithm>

using namespace std::literals;

namespace Game {

	LuaLevel::LuaLevel(const ResourceLoader& loader, std::string_view scriptName, DefaultCache& resourceCache, const TLVReader& reader, const Player& player, MessageBus& bus)
		: m_Script{ loader.Load(scriptName).AsString() }
		, m_Entities{}
		, m_Floor{
			resourceCache.Get<Mesh>("floor"),
			resourceCache.Get<Material>("floor"),
			{0.0f, -2.0f, 0.0f},
			{100.0f, 1.0f, 100.0f},
			std::vector<const Texture*>{resourceCache.Get<Texture>("floor_albedo"), resourceCache.Get<Texture>("floor_albedo")}
		}
		, m_Skybox{ reader, {{ "right", "left", "top", "bottom", "front", "back" }} }
		, m_SkyboxSampler{}
		, m_ResourceCache{ resourceCache }
		, m_Bus{ bus }
		, m_BarrelInfo{}
		, m_PS{}
		, m_Shapes{}
	{
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
			const auto info = runner.Execute<vec3, vec3, float>("barrel_info", i + 1ll);

			const auto& ent = m_Entities.emplace_back(resourceCache.Get<Mesh>("barrel"), resourceCache.Get<Material>("barrel"), std::get<0>(info), vec3{ 1.0f }, barrelTextures);

			const auto aabb = ent.GetBoundingBox();
			const auto halfExtents = (aabb.max - aabb.min) / 2.0f; // + aabb.min
			auto* shape = m_PS.CreateShape<BoxShape>(vec3{ halfExtents.x, halfExtents.y, halfExtents.z });
			m_Shapes.push_back(shape);
		}

		m_Scene = Scene{
			.entities = m_Entities | std::views::transform([](auto& e) { return std::addressof(e); }) | std::ranges::to<std::vector>(),
			.ambient = {0.3f, 0.3f, 0.3f},
			.directionalLight = {.direction = {-1.0f, -1.0f, -1.0f}, .color = {0.5f, 0.5f, 0.5f}},
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

		m_Scene.entities.push_back(&m_Floor);

		Restart();
	}

	void LuaLevel::Update(const Player& player)
	{
		const ScriptRunner runner{ m_Script };

		for (const auto& [index, entity] : std::views::enumerate(m_Entities))
		{
			runner.Execute("set_barrel_visibility", index + 1ll, entity.IsVisible());
		}

		runner.Execute("update_level", player.GetPosition());

		const auto transformedShapes =
			std::views::zip_transform(
				[](const auto& shape, const auto& entity)
				{
					return TransformedShape{ shape, {entity.GetPosition(), {1.0f, 1.0f, 1.0f}, {}} };
				},
				m_Shapes, m_Entities
			) | std::ranges::to<std::vector>();
		const auto max = std::ranges::size(transformedShapes);

		for (const auto& [index, entity] : std::views::enumerate(m_Entities))
		{
			const auto& [position, color, tintAmount] = runner.Execute<vec3, vec3, float>("barrel_info", index + 1ll);

			entity.SetPosition(position);

			m_BarrelInfo[std::addressof(entity)] = {
				.tintColor = { color.x, color.y, color.z },
				.tintAmount = tintAmount
			};

			auto combs = std::views::iota(0ull, max) |
						 std::views::transform(
						 [max](auto x)
						 {
								 return std::views::iota(x + 1ull, max) |
									 std::views::transform([x](auto y) { return std::make_pair(x, y); });
						 }) | std::views::join;

			for (const auto [i, j] : combs)
			{
				const auto& transformShape1 = transformedShapes[i];
				const auto& transformShape2 = transformedShapes[j];

				transformShape1.Intersects(transformShape2);
			}

			if (runner.Execute<bool>("is_complete"))
			{
				m_Bus.PostLevelComplete("lua_level");
			}
		}

		m_PS.Debug_Renderer().Clear();
		for (const auto& e : transformedShapes)
		{
			e.Draw(m_PS.Debug_Renderer());
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
	}

	std::span<const Entity> LuaLevel::GetEntities() const
	{
		return m_Entities;
	}

}
