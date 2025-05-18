#pragma once

#include "Core/ResourceCache.h"
#include "Core/ResourceLoader.h"
#include "Game/Levels/Level.h"
#include "Game/TransformedEntity.h"
#include "LuaScript.h"

#include <string_view>
#include <vector>

namespace Game {

	class LuaLevel : public Level
	{
	public:
		LuaLevel(const ResourceLoader& loader, std::string_view scriptName, DefaultCache& resourceCache, const TLVReader& reader);

		void Update(const Player& player) override;
		void Restart() override;

	private:
		LuaScipt m_Script;
		std::vector<TransformedEntity> m_Entities;
		Entity m_Floor;
		CubeMap m_Skybox;
		Sampler m_SkyboxSampler;
		DefaultCache& m_ResourceCache;
	};

}
