#include "Entity.h"

#include "Renderer/Mesh.h"

#include <ranges>
#include <algorithm>

namespace {

	Game::AABB CalculateBoundingBox(const Game::Mesh* mesh, const Game::Transform& transform)
	{
		const auto minmaxX = std::ranges::minmax(mesh->GetMeshData().vertices, std::less<>{}, [](const auto& e) { return e.position.x; });
		const auto minmaxY = std::ranges::minmax(mesh->GetMeshData().vertices, std::less<>{}, [](const auto& e) { return e.position.y; });
		const auto minmaxZ = std::ranges::minmax(mesh->GetMeshData().vertices, std::less<>{}, [](const auto& e) { return e.position.z; });

		const Game::mat4 transformMat{ transform };

		Game::AABB boundingBox = {
			transformMat * Game::vec3{minmaxX.min.position.x, minmaxY.min.position.y, minmaxZ.min.position.z},
			transformMat * Game::vec3{minmaxX.max.position.x, minmaxY.max.position.y, minmaxZ.max.position.z}
		};

		return boundingBox;
	}

}

namespace Game {

	Entity::Entity(const Mesh* mesh, const Material* material, const vec3& position, const vec3& scale, std::span<const Texture* const> textures)
		: m_Mesh(mesh)
		, m_Material(material)
		, m_Textures(std::ranges::cbegin(textures), std::ranges::cend(textures))
		, m_Transform(position, scale, {})
		, m_Visible(true)
		, m_BoundingBox(CalculateBoundingBox(m_Mesh, m_Transform))
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

	AABB Entity::GetBoundingBox() const
	{
		return m_BoundingBox;
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
		m_BoundingBox.min += translation;
		m_BoundingBox.max += translation;
	}

}
