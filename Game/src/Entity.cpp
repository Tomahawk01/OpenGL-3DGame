#include "Entity.h"

namespace Game {

	Entity::Entity(const Mesh* mesh, const Material* material, const vec3& position, const vec3& scale, const std::vector<std::tuple<const Texture*, const Sampler*>>& textures)
		: m_Mesh(mesh)
		, m_Material(material)
		, m_Textures(textures)
		, m_Model(mat4{ position } * mat4{ scale, {} })
	{}

	const Mesh* Entity::GetMesh() const
	{
		return m_Mesh;
	}

	const Material* Entity::GetMaterial() const
	{
		return m_Material;
	}

	std::span<const std::tuple<const Texture*, const Sampler*>> Entity::GetTextures() const
	{
		return m_Textures;
	}

	const Sampler* Entity::GetSampler() const
	{
		return m_Sampler;
	}

	const mat4& Entity::GetModel() const
	{
		return m_Model;
	}

}
