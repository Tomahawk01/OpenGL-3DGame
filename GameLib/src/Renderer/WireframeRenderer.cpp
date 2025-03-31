#include "WireframeRenderer.h"

#include <ranges>

namespace Game {

	void WireframeRenderer::Draw(const Camera&)
	{
	}

	void WireframeRenderer::Draw(const AABB& aabb)
	{
		m_Lines.push_back({ {aabb.max.x, aabb.max.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.max.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.max.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.max.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.max.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.max.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.max.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.max.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });

		m_Lines.push_back({ {aabb.max.x, aabb.max.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.min.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.max.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.min.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.max.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.min.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.max.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.min.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });

		m_Lines.push_back({ {aabb.max.x, aabb.min.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.min.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.min.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.min.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.min.x, aabb.min.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.min.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.min.y, aabb.min.z }, { 0.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ {aabb.max.x, aabb.min.y, aabb.max.z }, { 0.0f, 1.0f, 0.0f } });
	}

	std::vector<LineData> WireframeRenderer::yield()
	{
		const auto temp = m_Lines;
		m_Lines.clear();
		return temp;
	}

}
