#include "WireframeRenderer.h"

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include "Game/FrustumPlane.h"

#include <ranges>

namespace Game {

	void WireframeRenderer::Draw(const vec3& start, const vec3& end, const Color& color)
	{
		m_Lines.push_back({ start, color });
		m_Lines.push_back({ end, color });
	}

	void WireframeRenderer::Draw(const Camera& camera)
	{
		const auto corners{ camera.FrustumCorners() };

		const Color nearColor{ 1.0f, 0.0f, 1.0f };
		const Color farColor{ 1.0f, 0.0f, 0.0f };
		const Color connectColor{ 1.0f, 1.0f, 0.0f };

		Draw(corners[0], corners[1], nearColor);
		Draw(corners[1], corners[2], nearColor);
		Draw(corners[2], corners[3], nearColor);
		Draw(corners[3], corners[0], nearColor);

		Draw(corners[4], corners[5], farColor);
		Draw(corners[5], corners[6], farColor);
		Draw(corners[6], corners[7], farColor);
		Draw(corners[7], corners[4], farColor);

		Draw(corners[0], corners[4], connectColor);
		Draw(corners[1], corners[5], connectColor);
		Draw(corners[2], corners[6], connectColor);
		Draw(corners[3], corners[7], connectColor);
	}

	void WireframeRenderer::Draw(const std::array<FrustumPlane, 6u>& planes)
	{
		const FrustumPlane& near = planes[0];
		const FrustumPlane& far = planes[1];
		const FrustumPlane& left = planes[2];
		const FrustumPlane& right = planes[3];
		const FrustumPlane& bottom = planes[4];
		const FrustumPlane& top = planes[5];

		const vec3 near_left_top = Intersection(near, left, top);
		const vec3 near_right_top = Intersection(near, right, top);
		const vec3 near_left_bottom = Intersection(near, left, bottom);
		const vec3 near_right_bottom = Intersection(near, right, bottom);
		const vec3 far_left_top = Intersection(far, left, top);
		const vec3 far_right_top = Intersection(far, right, top);
		const vec3 far_left_bottom = Intersection(far, left, bottom);
		const vec3 far_right_bottom = Intersection(far, right, bottom);

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
