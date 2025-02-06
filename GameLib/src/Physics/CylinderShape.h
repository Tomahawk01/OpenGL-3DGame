#pragma once

#include "Utilities/PassKey.h"
#include "Shape.h"

#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#include <format>

namespace Game {

	class PhysicsSystem;

	class CylinderShape : public Shape
	{
	public:
		CylinderShape(float halfHeight, float radius, PassKey<PhysicsSystem>);

		float GetHalfHeight() const;
		float GetRadius() const;

		const ::JPH::ShapeSettings* GetNativeHandle() const override;

	private:
		float m_HalfHeight;
		float m_Radius;
		::JPH::CylinderShapeSettings m_CylinderShapeSettings;
	};

}

template<>
struct std::formatter<Game::CylinderShape>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::CylinderShape& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "CylinderShape: {} {}", obj.GetHalfHeight(), obj.GetRadius());
	}
};
