#include "Shape.h"

namespace Game {

	Shape::Shape(ShapeType type, PassKey<PhysicsSystem>)
		: m_Type(type)
	{}

	ShapeType Shape::GetType() const
	{
		return m_Type;
	}

}
