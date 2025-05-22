#include "LevelAlpha.h"

#include "Messaging/MessageBus.h"
#include "Core/ResourceCache.h"

#include "Game/TransformedEntity.h"
#include "Game/Player.h"

namespace {

	bool IntersectsFrustum(const Game::AABB& aabb, const std::array<Game::FrustumPlane, 6u>& planes)
	{
		for (const auto& plane : planes)
		{
			Game::vec3 positiveVertex = aabb.min;
			if (plane.normal.x >= 0)
				positiveVertex.x = aabb.max.x;
			if (plane.normal.y >= 0)
				positiveVertex.y = aabb.max.y;
			if (plane.normal.z >= 0)
				positiveVertex.z = aabb.max.z;

			if (Game::vec3::Dot(plane.normal, positiveVertex) + plane.distance < 0.0f)
				return false;
		}

		return true;
	}

	constexpr auto CameraDelta = [](const Game::vec3& in, const Game::GameTransformState& state) -> Game::TransformerResult
	{
		return { in + (state.camera.GetPosition() - state.lastCameraPos) };
	};

	constexpr auto Invert = [](const Game::vec3& in, const Game::GameTransformState&) -> Game::TransformerResult
	{
		return { -in };
	};

	constexpr auto CheckVisible = [](const Game::vec3& in, const Game::GameTransformState& state) -> Game::TransformerResult
	{
		const auto planes = state.camera.FrustumPlanes();
		return { in, !IntersectsFrustum(state.aabb, planes) };
	};

}

namespace Game {

	LevelAlpha::LevelAlpha(DefaultCache& resourceCache, const TLVReader& reader, const Player& player, MessageBus& bus)
		: m_Entities{}
		, m_Floor{
			resourceCache.Get<Mesh>("floor"),
			resourceCache.Get<Material>("floor"),
			{0.0f, -2.0f, 0.0f},
			{100.0f, 1.0f, 100.0f},
			std::vector<const Texture*>{resourceCache.Get<Texture>("floor_albedo"), resourceCache.Get<Texture>("floor_albedo")} }
		, m_Skybox{ reader, {{ "right", "left", "top", "bottom", "front", "back" }} }
		, m_SkyboxSampler{}
		, m_State{ player.GetCamera(), {}, player.GetCamera().GetPosition() }
		, m_Bus{ bus }
		, m_ResourceCache{ resourceCache }
	{
		const Texture* barrelTextures[]{
			resourceCache.Get<Texture>("barrel_albedo"),
			resourceCache.Get<Texture>("barrel_specular"),
			resourceCache.Get<Texture>("barrel_normal")
		};

		m_Entities.emplace_back(
			Entity{ resourceCache.Get<Mesh>("barrel"), resourceCache.Get<Material>("barrel"), {}, {1.0f}, barrelTextures},
			AABB{ {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f} },
			std::make_unique<Chain<GameTransformState>>());
		m_Entities.emplace_back(
			Entity{ resourceCache.Get<Mesh>("barrel"), resourceCache.Get<Material>("barrel"), {5.0f, 0.0f, 0.0f}, {1.0f}, barrelTextures },
			AABB{ {3.0f, -1.0f, -1.0f}, {5.0f, 1.0f, 1.0f} },
			std::make_unique<Chain<GameTransformState, CheckVisible, CameraDelta>>());

		m_Scene = Scene{
			.entities = m_Entities | std::views::transform([](auto& e) { return std::addressof(e.entity); }) | std::ranges::to<std::vector>(),
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

	void LevelAlpha::Update(const Player& player)
	{
		for (auto& transformedEntity : m_Entities)
		{
			auto& [entity, aabb, transformer] = transformedEntity;

			m_State.aabb = aabb;
			const vec3 entityDelta = transformer->Go({}, m_State);
			entity.Translate(entityDelta);
			aabb.min += entityDelta;
			aabb.max += entityDelta;
		}

		m_State.lastCameraPos = player.GetCamera().GetPosition();

		if (vec3::Distance(m_Entities[0].entity.GetPosition(), m_Entities[1].entity.GetPosition()) < 1.0f)
		{
			m_Bus.PostLevelComplete("alpha");
		}
	}

	void LevelAlpha::Restart()
	{
		m_ResourceCache.Get<Material>("barrel")->SetUniformCallback([this](const Material* material, const Entity* entity)
		{
			const float tintAmount = entity == std::addressof(m_Entities[0].entity) ? 1.0f : 0.5f;
			material->SetUniform("tint_color", Color{ 0.0f, 0.0f, 1.0f });
			material->SetUniform("tint_amount", tintAmount);
		});
	}

}
