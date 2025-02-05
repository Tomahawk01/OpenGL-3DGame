#pragma once

#include "Utilities/PassKey.h"
#include "Shape.h"

#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <format>

namespace Game {

	class PhysicsSystem;

	class SphereShape : public Shape
	{
	public:
		SphereShape(float radius, PassKey<PhysicsSystem>);

		float GetRadius() const;

		const ::JPH::ShapeSettings* GetNativeHandle() const override;

	private:
		float m_Radius;
		::JPH::SphereShapeSettings m_SphereShapeSettings;
	};

}

template<>
struct std::formatter<Game::SphereShape>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::SphereShape& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "SphereShape: {}", obj.GetRadius());
	}
};
