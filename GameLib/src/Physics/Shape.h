#pragma once

#include "Utilities/PassKey.h"

#if !defined(JPH_DEBUG_RENDERER)
#define JPH_DEBUG_RENDERER 1
#endif
#include <Jolt/Jolt.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

namespace Game {

	enum class ShapeType
	{
		BOX,
		CYLINDER,
		SPHERE
	};

	class PhysicsSystem;

	class Shape
	{
	public:
		Shape(ShapeType type, PassKey<PhysicsSystem>);
		virtual ~Shape() = default;

		virtual const ::JPH::Shape* GetNativeHandle() const = 0;

		ShapeType GetType() const;

	private:
		ShapeType m_Type;
	};

}
