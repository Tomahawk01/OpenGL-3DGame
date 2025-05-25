#include "LuaLevel.h"

#include "Scripting/ScriptRunner.h"

#include <ranges>

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
			const vec3 position = runner.Execute<vec3>("barrel_position", i + 1ll);

			m_Entities.emplace_back(resourceCache.Get<Mesh>("barrel"), resourceCache.Get<Material>("barrel"), position, vec3{1.0f}, barrelTextures);
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
	}

	void LuaLevel::Update(const Player& player)
	{
		const ScriptRunner runner{ m_Script };

		for (const auto& [index, entity] : std::views::enumerate(m_Entities))
		{
			runner.Execute("set_barrel_visibility", index + 1ll, entity.IsVisible());
		}

		runner.Execute("update_level", player.GetPosition());

		for (const auto& [index, entity] : std::views::enumerate(m_Entities))
		{
			const auto position = runner.Execute<vec3>("barrel_position", index + 1ll);

			entity.SetPosition(position);
		}

		if (runner.Execute<bool>("is_complete"))
		{
			m_Bus.PostLevelComplete("lua_level");
		}
	}

	void LuaLevel::Restart()
	{
		const ScriptRunner runner{ m_Script };
		runner.Execute("restart_level");
	}

	std::span<const Entity> LuaLevel::GetEntities() const
	{
		return m_Entities;
	}

}
