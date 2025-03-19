#include "Entity.h"

namespace Game {

	Entity::Entity(const Mesh* mesh, const Material* material, const vec3& position, const vec3& scale, const std::span<const Texture*>& textures)
		: m_Mesh(mesh)
		, m_Material(material)
		, m_Textures(std::ranges::cbegin(textures), std::ranges::cend(textures))
		, m_Transform(position, scale, {})
	{}

	const Mesh* Entity::GetMesh() const
	{
		return m_Mesh;
	}

	const Material* Entity::GetMaterial() const
	{
		return m_Material;
	}

	std::span<const Texture* const> Entity::GetTextures() const
	{
		return m_Textures;
	}

	const Sampler* Entity::GetSampler() const
	{
		return m_Sampler;
	}

	const Transform& Entity::GetTransform() const
	{
		return m_Transform;
	}

	void Entity::SetPosition(const vec3& position)
	{
		m_Transform.Position = position;
	}

	void Entity::SetRotation(const quat& rotation)
	{
		m_Transform.Rotation = rotation;
	}

}
