#pragma once

#include "Utilities/PassKey.h"
#include "Renderer/LineData.h"

#if !defined(JPH_DEBUG_RENDERER)
#define JPH_DEBUG_RENDERER 1
#endif
#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

#include <string_view>
#include <vector>
#include <span>

namespace Game {

	class PhysicsSystem;

	class DebugRenderer : public ::JPH::DebugRendererSimple
	{
	public:
		DebugRenderer(PassKey<PhysicsSystem>);

		virtual void DrawLine(::JPH::RVec3Arg from, ::JPH::RVec3Arg to, ::JPH::ColorArg color) override;
		virtual void DrawTriangle(::JPH::RVec3Arg v1, ::JPH::RVec3Arg v2, ::JPH::RVec3Arg v3, ::JPH::ColorArg color, ECastShadow) override;
		virtual void DrawText3D(::JPH::RVec3Arg, const std::string_view& str, ::JPH::ColorArg, float) override;

		void Clear();

		std::span<const LineData> GetLines() const;

	private:
		std::vector<LineData> m_Lines;
	};

}
