#include "MeshShape.h"

#include "Utilities/Error.h"
#include "JoltUtils.h"

#include <Jolt/Core/Array.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Geometry/IndexedTriangle.h>

#include <ranges>

namespace {

	::JPH::Ref<::JPH::Shape> CreateMeshShape(Game::MeshData meshData, float meshScale)
	{
		const auto joltVertexList = meshData.vertices
			| std::views::transform([meshScale](const auto& e) { return ::JPH::Float3{ e.position.x * meshScale, e.position.y * meshScale, e.position.z * meshScale }; })
			| std::ranges::to<::JPH::Array<::JPH::Float3>>();
		const auto joltIndexList = meshData.indices
			| std::views::chunk(3u)
			| std::views::transform([](const auto& e) { return ::JPH::IndexedTriangle{ e[0], e[1], e[2] }; })
			| std::ranges::to<::JPH::Array<::JPH::IndexedTriangle>>();

		const ::JPH::MeshShapeSettings meshSettings{ joltVertexList, joltIndexList };
		const auto shape = meshSettings.Create();
		Game::Ensure(!shape.HasError(), "Failed to create mesh shape");

		return shape.Get();
	}

}

namespace Game {

	MeshShape::MeshShape(MeshData meshData, float meshScale, PassKey<PhysicsSystem> passKey)
		: Shape(ShapeType::BOX, passKey)
		, m_MeshShape{ CreateMeshShape(meshData, meshScale) }
	{}

	const ::JPH::Shape* MeshShape::GetNativeHandle() const
	{
		return m_MeshShape.GetPtr();
	}

	std::string MeshShape::to_string() const
	{
		return "MeshShape";
	}

}
