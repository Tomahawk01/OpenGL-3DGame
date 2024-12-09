#include "Entity.h"

namespace Game {

	Entity::Entity(const Mesh* mesh, const Material* material, const vec3& position, const std::vector<const Texture*>& textures, const Sampler* sampler)
		: m_Mesh(mesh)
		, m_Material(material)
		, m_Textures(textures)
		, m_Sampler(sampler)
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

	const std::vector<const Texture*>& Entity::GetTextures() const
	{
		return m_Textures;
	}

	const Sampler* Entity::GetSampler() const
	{
		return m_Sampler;
	}

	std::span<const float> Entity::GetModel() const
	{
		return m_Model.data();
	}

}
