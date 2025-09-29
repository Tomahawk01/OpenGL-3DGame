#pragma once

#include "Shape.h"
#include "Math/Transform.h"
#include "Math/Color.h"
#include "DebugRenderer.h"

#include <optional>

namespace Game {

	struct CollisionResult
	{
		vec3 contact1;
		vec3 contact2;
		vec3 penetrationAxis;
		float penetrationDepth;
	};

	class TransformedShape
	{
	public:
		TransformedShape(const Shape* shape, const Transform& transform);

		std::optional<CollisionResult> Intersects(const TransformedShape& shape) const;

		void Draw(DebugRenderer& debugRenderer, const Color& color) const;
		void Translate(const vec3& translation);

		const Shape* GetShape() const;
		const Transform& GetTransform() const;

	private:
		const Shape* m_Shape;
		Transform m_Transform;
	};

}
