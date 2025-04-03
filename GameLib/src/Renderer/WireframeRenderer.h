#pragma once

#include "Camera.h"
#include "LineData.h"

#include "Game/AABB.h"
#include "Game/FrustumPlane.h"

#include <vector>
#include <array>

namespace Game {

	class WireframeRenderer
	{
	public:
		void Draw(const Camera& camera);
		void Draw(const std::array<FrustumPlane, 6u>& planes);
		void Draw(const AABB& aabb);

		std::vector<LineData> yield();

	private:
		std::vector<LineData> m_Lines;
	};

}
