#include "Entity.h"

namespace Game {

	Entity::Entity(const Mesh* mesh, const Material* material, const vec3& position, const Texture* texture, const Sampler* sampler)
		: m_Mesh(mesh)
		, m_Material(material)
		, m_Texture(texture)
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

	const Texture* Entity::GetTexture() const
	{
		return m_Texture;
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
