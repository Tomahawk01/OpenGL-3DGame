#pragma once

#include "Utilities/PassKey.h"
#include "Shape.h"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

namespace Game {

	class PhysicsSystem;

	class CapsuleShape : public Shape
	{
	public:
		CapsuleShape(float halfHeight, float radius, PassKey<PhysicsSystem> passKey);

		float GetHalfHeight() const;
		float GetRadius() const;

		const ::JPH::Shape* GetNativeHandle() const override;

		std::string to_string() const;

	private:
		float m_HalfHeight;
		float m_Radius;
		::JPH::CapsuleShape m_CapsuleShape;
	};

}
