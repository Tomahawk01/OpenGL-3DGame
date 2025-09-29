#include "TransformedShape.h"

#include "Utilities/Logger.h"
#include "Utilities/Error.h"
#include "JoltUtils.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CollisionDispatch.h>

namespace {

	struct SimpleCollisionCollector : ::JPH::CollideShapeCollector
	{
		SimpleCollisionCollector(bool& hit)
			: Hit(hit)
		{}

		void OnBody(const ::JPH::Body&) override
		{
		}

		void AddHit(const ResultType&) override
		{
			Hit = true;
		}

		bool& Hit;
	};

}

namespace Game {

	TransformedShape::TransformedShape(const Shape* shape, const Transform& transform)
		: m_Shape{ shape }
		, m_Transform{ transform }
	{
		Expect(shape != nullptr, "Must provide a Shape");
	}

	bool TransformedShape::Intersects(const TransformedShape& shape) const
	{
		::JPH::CollideShapeSettings settings{};
		bool hit = false;
		SimpleCollisionCollector collector{ hit };
		const auto transform1 = ToJolt(m_Transform);
		const auto transform2 = ToJolt(shape.m_Transform);
		const ::JPH::SubShapeIDCreator subshapeIDCreator1{};
		const ::JPH::SubShapeIDCreator subshapeIDCreator2{};

		::JPH::CollisionDispatch::sCollideShapeVsShape(
			m_Shape->GetNativeHandle(), shape.m_Shape->GetNativeHandle(),
			ToJolt(m_Transform.Scale), ToJolt(shape.m_Transform.Scale),
			transform1, transform2,
			subshapeIDCreator1, subshapeIDCreator2,
			settings,
			collector);

		return hit;
	}

	void TransformedShape::Draw(DebugRenderer& debugRenderer, const Color& color) const
	{
		m_Shape->GetNativeHandle()->Draw(&debugRenderer, ToJolt(m_Transform), ToJolt(m_Transform.Scale), ToJolt(color), false, true);
	}

	void TransformedShape::Translate(const vec3& translation)
	{
		m_Transform.Position += translation;
	}

	const Shape* TransformedShape::GetShape() const
	{
		return m_Shape;
	}

	const Transform& TransformedShape::GetTransform() const
	{
		return m_Transform;
	}

}
