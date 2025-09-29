#include "TransformedShape.h"

#include "Utilities/Logger.h"
#include "Utilities/Error.h"
#include "JoltUtils.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CollisionDispatch.h>

namespace {

	struct SimpleCollisionCollector : ::JPH::CollideShapeCollector
	{
		SimpleCollisionCollector()
		{}

		void OnBody(const ::JPH::Body&) override
		{
		}

		void AddHit(const ResultType& joltResult) override
		{
			collisionResult = Game::CollisionResult{
				.contact1 = Game::ToNative(joltResult.mContactPointOn1),
				.contact2 = Game::ToNative(joltResult.mContactPointOn2),
				.penetrationAxis = Game::ToNative(joltResult.mPenetrationAxis),
				.penetrationDepth = joltResult.mPenetrationDepth
			};
		}

		std::optional<Game::CollisionResult> collisionResult = std::nullopt;
	};

}

namespace Game {

	TransformedShape::TransformedShape(const Shape* shape, const Transform& transform)
		: m_Shape{ shape }
		, m_Transform{ transform }
	{
		Expect(shape != nullptr, "Must provide a Shape");
	}

	std::optional<CollisionResult> TransformedShape::Intersects(const TransformedShape& shape) const
	{
		::JPH::CollideShapeSettings settings{};
		SimpleCollisionCollector collector{};
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

		return collector.collisionResult;
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
