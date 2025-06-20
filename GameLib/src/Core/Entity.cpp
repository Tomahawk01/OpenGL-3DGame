#include "Entity.h"

#include "Renderer/Mesh.h"

#include <ranges>
#include <algorithm>

namespace Game {

	Entity::Entity(const Mesh* mesh,
				   const Material* material,
				   const vec3& position,
				   const vec3& scale,
				   std::span<const Texture* const> textures,
				   TransformedShape boundingBox,
				   uint32_t collisionLayer,
				   uint32_t collisionMask)
		: m_Mesh(mesh)
		, m_Material(material)
		, m_Textures(std::ranges::cbegin(textures), std::ranges::cend(textures))
		, m_Transform(position, scale, {})
		, m_Visible(true)
		, m_BoundingBox(std::move(boundingBox))
		, m_CollisionLayer(collisionLayer)
		, m_CollisionMask(collisionMask)
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

	bool Entity::IsVisible() const
	{
		return m_Visible;
	}

	void Entity::SetVisibility(bool visible)
	{
		m_Visible = visible;
	}

	const TransformedShape& Entity::GetBoundingBox() const
	{
		return m_BoundingBox;
	}

	uint32_t Entity::GetCollisionLayer() const
	{
		return m_CollisionLayer;
	}

	uint32_t Entity::GetCollisionMask() const
	{
		return m_CollisionMask;
	}

	vec3 Entity::GetPosition() const
	{
		return m_Transform.Position;
	}

	void Entity::SetPosition(const vec3& position)
	{
		const vec3 delta = position - m_Transform.Position;
		Translate(delta);
	}

	void Entity::SetRotation(const quat& rotation)
	{
		m_Transform.Rotation = rotation;
	}

	void Entity::Translate(const vec3& translation)
	{
		m_Transform.Position += translation;
		m_BoundingBox.Translate(translation);
	}

}
