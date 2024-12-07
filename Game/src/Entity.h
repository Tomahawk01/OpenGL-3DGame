#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include <span>

namespace Game {

	class Material;
	class Mesh;

	class Entity
	{
	public:
		Entity(const Mesh* mesh, const Material* material, const vec3& position);

		const Mesh* GetMesh() const;
		const Material* GetMaterial() const;
		std::span<const float> GetModel() const;

	private:
		const Mesh* m_Mesh;
		const Material* m_Material;

		mat4 m_Model;
	};

}
