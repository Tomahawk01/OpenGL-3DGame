#include "DebugRenderer.h"

#include "Utilities/Logger.h"

#include <limits>

namespace {

	Game::vec3 toNative(::JPH::RVec3Arg v)
	{
		return { v.GetX(), v.GetY(), v.GetZ() };
	}

	Game::Color toNative(::JPH::ColorArg c)
	{
		const auto toFloat = []<std::integral T>(T b) { return static_cast<float>(b) / static_cast<float>(std::numeric_limits<T>::max()); };
		return { toFloat(c.r), toFloat(c.g), toFloat(c.b) };
	}

}

namespace Game {

	void DebugRenderer::DrawLine(::JPH::RVec3Arg from, ::JPH::RVec3Arg to, ::JPH::ColorArg color)
	{
		m_Lines.push_back({ toNative(from), toNative(color) });
		m_Lines.push_back({ toNative(to), toNative(color) });
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
