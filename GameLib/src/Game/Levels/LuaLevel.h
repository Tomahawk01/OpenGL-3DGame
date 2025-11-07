#pragma once

#include "Core/ResourceCache.h"
#include "Core/Entity.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"
#include "Physics/PhysicsSystem.h"
#include "Scripting/LuaScript.h"
#include "Scripting/ScriptLoader.h"
#include "Messaging/MessageBus.h"
#include "Messaging/Subscriber.h"
#include "Messaging/AutoSubscribe.h"
#include "TLV/TLVReader.h"

#include "Game/Levels/Level.h"
#include "Game/Player.h"

#include <vector>
#include <unordered_map>
#include <span>

namespace Game {

	enum class LevelState
	{
		PLAYING,
		COMPLETE,
		LOST
	};

	class LuaLevel : public Level, public Subscriber
	{
	public:
		struct BarrelInfo
		{
			Color tintColor;
			float tintAmount;
		};

		LuaLevel(PhysicsSystem& ps, const ScriptLoader& loader, DefaultCache& resourceCache, const TLVReader& reader, const Player& player, MessageBus& bus);

		void Update(Player& player) override;
		void Restart() override;
		void HandleEntityIntersect(const Entity* a, const Entity* b) override;
		void ToggleRenderDebugLines();

		std::span<const Entity> GetEntities() const;

	private:
		void UpdateBarrelVisibility();
		void UpdateLuaLevel(Player& player);

	private:
		PhysicsSystem &m_PS;
		LuaScript m_Script;
		std::vector<Entity> m_Entities;
		std::vector<Entity> m_LevelEntities;
		std::vector<Shape*> m_LevelPhysicsShapes;
		CubeMap m_Skybox;
		DefaultCache& m_ResourceCache;
		MessageBus& m_Bus;
		std::unordered_map<const Entity*, BarrelInfo> m_BarrelInfo;
		std::vector<const Shape*> m_Shapes;
		AutoSubscribe m_AutoSub;
		bool m_RenderDebugLines;
	};

}
