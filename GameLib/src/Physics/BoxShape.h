#pragma once

#include "Utilities/PassKey.h"
#include "Math/Vector3.h"
#include "Shape.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include <format>

namespace Game {

	class PhysicsSystem;

	class BoxShape : public Shape
	{
	public:
		BoxShape(const vec3& dimensions, PassKey<PhysicsSystem>);

		vec3 GetDimensions() const;

		const ::JPH::ShapeSettings* GetNativeHandle() const override;

	private:
		vec3 m_Dimensions;
		::JPH::BoxShapeSettings m_BoxShapeSettings;
	};

}

template<>
struct std::formatter<Game::BoxShape>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::BoxShape& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "BoxShape: {}", obj.GetDimensions());
	}
};
