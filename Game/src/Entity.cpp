#include "Entity.h"

namespace Game {

	Entity::Entity(const Mesh* mesh, const Material* material, const vec3& position)
		: m_Mesh(mesh)
		, m_Material(material)
		, m_Model(position)
	{}

	const Mesh* Entity::GetMesh() const
	{
		return m_Mesh;
	}

	const Material* Entity::GetMaterial() const
	{
		return m_Material;
	}

	std::span<const float> Entity::GetModel() const
	{
		return m_Model.data();
	}

}
