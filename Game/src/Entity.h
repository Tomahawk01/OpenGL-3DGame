#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include <vector>
#include <tuple>

namespace Game {

	class Material;
	class Mesh;
	class Texture;
	class Sampler;

	class Entity
	{
	public:
		Entity(const Mesh* mesh,
			   const Material* material,
			   const vec3& position,
			   const vec3& scale,
			   const std::vector<std::tuple<const Texture*, const Sampler*>>& textures);

		const Mesh* GetMesh() const;
		const Material* GetMaterial() const;
		std::span<const std::tuple<const Texture*, const Sampler*>> GetTextures() const;
		const Sampler* GetSampler() const;
		const mat4& GetModel() const;

	private:
		const Mesh* m_Mesh;
		const Material* m_Material;
		std::vector<std::tuple<const Texture*, const Sampler*>> m_Textures;
		const Sampler* m_Sampler;

		mat4 m_Model;
	};

}
