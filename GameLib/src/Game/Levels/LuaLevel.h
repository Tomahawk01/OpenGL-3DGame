#pragma once

#include "Core/ResourceCache.h"
#include "Core/ResourceLoader.h"
#include "Core/Entity.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"
#include "Physics/PhysicsSystem.h"
#include "Scripting/LuaScript.h"
#include "Messaging/MessageBus.h"
#include "TLV/TLVReader.h"

#include "Game/Levels/Level.h"
#include "Game/Player.h"

#include <string_view>
#include <vector>
#include <unordered_map>
#include <span>

namespace Game {

	class LuaLevel : public Level
	{
	public:
		struct BarrelInfo
		{
			Color tintColor;
			float tintAmount;
		};

		LuaLevel(const ResourceLoader& loader, std::string_view scriptName, DefaultCache& resourceCache, const TLVReader& reader, const Player& player, MessageBus& bus);

		void Update(const Player& player) override;
		void Restart() override;

		std::span<const Entity> GetEntities() const;

	private:
		LuaScript m_Script;
		std::vector<Entity> m_Entities;
		Entity m_Floor;
		CubeMap m_Skybox;
		Sampler m_SkyboxSampler;
		DefaultCache& m_ResourceCache;
		MessageBus m_Bus;
		std::unordered_map<const Entity*, BarrelInfo> m_BarrelInfo;
		PhysicsSystem m_PS;
		std::vector<const Shape*> m_Shapes;
	};

}
