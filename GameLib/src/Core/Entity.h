#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Transform.h"
#include "Math/Quaternion.h"

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
			   const std::span<const Texture*>& textures);

		const Mesh* GetMesh() const;
		const Material* GetMaterial() const;
		std::span<const Texture* const> GetTextures() const;
		const Sampler* GetSampler() const;
		const Transform& GetTransform() const;

		void SetPosition(const vec3& position);
		void SetRotation(const quat& rotation);

	private:
		const Mesh* m_Mesh;
		const Material* m_Material;
		std::vector<const Texture*> m_Textures;
		const Sampler* m_Sampler;
		Transform m_Transform;
	};

}
