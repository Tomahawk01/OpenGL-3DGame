#include "WireframeRenderer.h"

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include "Game/FrustumPlane.h"

#include <ranges>

namespace Game {

	void WireframeRenderer::Draw(const Camera& camera)
	{
		Draw(camera.FrustumPlanes());
	}

	void WireframeRenderer::Draw(const std::array<FrustumPlane, 6u>& planes)
	{
		const Game::FrustumPlane& near = planes[0];
		const Game::FrustumPlane& far = planes[1];
		const Game::FrustumPlane& left = planes[2];
		const Game::FrustumPlane& right = planes[3];
		const Game::FrustumPlane& bottom = planes[4];
		const Game::FrustumPlane& top = planes[5];

		const Game::vec3 near_left_top = Intersection(near, left, top);
		const Game::vec3 near_right_top = Intersection(near, right, top);
		const Game::vec3 near_left_bottom = Intersection(near, left, bottom);
		const Game::vec3 near_right_bottom = Intersection(near, right, bottom);
		const Game::vec3 far_left_top = Intersection(far, left, top);
		const Game::vec3 far_right_top = Intersection(far, right, top);
		const Game::vec3 far_left_bottom = Intersection(far, left, bottom);
		const Game::vec3 far_right_bottom = Intersection(far, right, bottom);

		m_Lines.push_back({ near_left_top, { 1.0f, 0.0f, 1.0f } });
		m_Lines.push_back({ near_right_top, { 1.0f, 0.0f, 1.0f } });
		m_Lines.push_back({ near_right_top, { 1.0f, 0.0f, 1.0f } });
		m_Lines.push_back({ near_right_bottom, { 1.0f, 0.0f, 1.0f } });
		m_Lines.push_back({ near_right_bottom, { 1.0f, 0.0f, 1.0f } });
		m_Lines.push_back({ near_left_bottom, { 1.0f, 0.0f, 1.0f } });
		m_Lines.push_back({ near_left_bottom, { 1.0f, 0.0f, 1.0f } });
		m_Lines.push_back({ near_left_top, { 1.0f, 0.0f, 1.0f } });

		m_Lines.push_back({ near_left_top, { 1.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ far_left_top, { 1.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ near_right_top, { 1.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ far_right_top, { 1.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ near_right_bottom, { 1.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ far_right_bottom, { 1.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ near_left_bottom, { 1.0f, 1.0f, 0.0f } });
		m_Lines.push_back({ far_left_bottom, { 1.0f, 1.0f, 0.0f } });

		m_Lines.push_back({ far_left_top, { 1.0f, 0.0f, 0.0f } });
		m_Lines.push_back({ far_right_top, { 1.0f, 0.0f, 0.0f } });
		m_Lines.push_back({ far_right_top, { 1.0f, 0.0f, 0.0f } });
		m_Lines.push_back({ far_right_bottom, { 1.0f, 0.0f, 0.0f } });
		m_Lines.push_back({ far_right_bottom, { 1.0f, 0.0f, 0.0f } });
		m_Lines.push_back({ far_left_bottom, { 1.0f, 0.0f, 0.0f } });
		m_Lines.push_back({ far_left_bottom, { 1.0f, 0.0f, 0.0f } });
		m_Lines.push_back({ far_left_top, { 1.0f, 0.0f, 0.0f } });
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
