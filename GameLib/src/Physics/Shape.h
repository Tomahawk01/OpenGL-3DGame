#pragma once

#include "Utilities/PassKey.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

namespace Game {

	class PhysicsSystem;

	class Shape
	{
	public:
		Shape(PassKey<PhysicsSystem>);
		virtual ~Shape() = default;

		virtual const ::JPH::Shape* GetNativeHandle() const = 0;
	};

}
