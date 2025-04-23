#pragma once

#include "Level.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"
#include "Renderer/CubeMap.h"
#include "Renderer/Sampler.h"
#include "TLV/TLVReader.h"

#include "Game/TransformedEntity.h"

#include <span>
#include <vector>

namespace Game {

	class LevelAlpha : public Level
	{
	public:
		LevelAlpha(const Mesh* floorMesh, const Material* floorMaterial, std::span<const Texture*> floorTextures,
				   const Mesh* barrelMesh, const Material* barrelMaterial, std::span<const Texture*> barrelTextures,
				   const TLVReader& reader, const Player& player);
		~LevelAlpha() override = default;

		void Update(const Player& player) override;
		bool Complete() const override;

	private:
		std::vector<TransformedEntity> m_Entities;
		Entity m_Floor;
		CubeMap m_Skybox;
		Sampler m_SkyboxSampler;
		GameTransformState m_State;
	};

}
