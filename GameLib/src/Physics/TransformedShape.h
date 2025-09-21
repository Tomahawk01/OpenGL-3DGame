#pragma once

#include "Shape.h"
#include "Math/Transform.h"
#include "Math/Color.h"
#include "DebugRenderer.h"

namespace Game {

	class TransformedShape
	{
	public:
		TransformedShape(const Shape* shape, const Transform& transform);

		bool Intersects(const TransformedShape& shape) const;

		void Draw(DebugRenderer& debugRenderer, const Color& color) const;
		void Translate(const vec3& translation);

		const Shape* GetShape() const;
		const Transform& GetTransform() const;

	private:
		const Shape* m_Shape;
		Transform m_Transform;
	};

}
