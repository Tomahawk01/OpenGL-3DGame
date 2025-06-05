#pragma once

#include "Shape.h"
#include "Math/Transform.h"
#include "DebugRenderer.h"

namespace Game {

	class TransformedShape
	{
	public:
		TransformedShape(const Shape* shape, const Transform& transform);

		bool Intersects(const TransformedShape& shape) const;

		void Draw(DebugRenderer& debugRenderer) const;

	private:
		const Shape* m_Shape;
		Transform m_Transform;
	};

}
