#pragma once

#include "Level.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"
#include "Renderer/CubeMap.h"
#include "Renderer/Sampler.h"
#include "Core/ResourceCache.h"
#include "TLV/TLVReader.h"

#include "Game/TransformedEntity.h"

#include <span>
#include <vector>

namespace Game {

	class MessageBus;

	class LevelAlpha : public Level
	{
	public:
		LevelAlpha(ResourceCache& resourceCache,
				   const Material* floorMaterial, std::span<const Texture*> floorTextures,
				   Material* barrelMaterial, std::span<const Texture*> barrelTextures,
				   const TLVReader& reader, const Player& player, MessageBus& bus);
		~LevelAlpha() override = default;

		void Update(const Player& player) override;

	private:
		std::vector<TransformedEntity> m_Entities;
		Entity m_Floor;
		CubeMap m_Skybox;
		Sampler m_SkyboxSampler;
		GameTransformState m_State;
		MessageBus& m_Bus;
	};

}
