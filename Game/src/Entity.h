#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include <span>
#include <vector>

namespace Game {

	class Material;
	class Mesh;
	class Texture;
	class Sampler;

	class Entity
	{
	public:
		Entity(const Mesh* mesh, const Material* material, const vec3& position, const std::vector<const Texture*> &textures, const Sampler* sampler);

		const Mesh* GetMesh() const;
		const Material* GetMaterial() const;
		const std::vector<const Texture*>& GetTextures() const;
		const Sampler* GetSampler() const;
		std::span<const float> GetModel() const;

	private:
		const Mesh* m_Mesh;
		const Material* m_Material;
		std::vector<const Texture*> m_Textures;
		const Sampler* m_Sampler;

		mat4 m_Model;
	};

}
