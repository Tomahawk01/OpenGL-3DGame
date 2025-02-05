#include "DebugRenderer.h"

#include "Utilities/Logger.h"
#include "JoltUtils.h"

#include <limits>

namespace Game {

	DebugRenderer::DebugRenderer(PassKey<PhysicsSystem>)
	{}

	void DebugRenderer::DrawLine(::JPH::RVec3Arg from, ::JPH::RVec3Arg to, ::JPH::ColorArg color)
	{
		m_Lines.push_back({ ToNative(from), ToNative(color) });
		m_Lines.push_back({ ToNative(to), ToNative(color) });
	}

	void DebugRenderer::DrawTriangle(::JPH::RVec3Arg v1, ::JPH::RVec3Arg v2, ::JPH::RVec3Arg v3, ::JPH::ColorArg color, ECastShadow)
	{
		DrawLine(v1, v2, color);
		DrawLine(v2, v3, color);
		DrawLine(v3, v1, color);
	}

	void DebugRenderer::DrawText3D(::JPH::RVec3Arg, const std::string_view& str, ::JPH::ColorArg, float)
	{
		Logger::Info("Debug text {}", str);
	}

	void DebugRenderer::Clear()
	{
		m_Lines.clear();
	}

	std::span<const LineData> DebugRenderer::GetLines() const
	{
		return m_Lines;
	}

}
