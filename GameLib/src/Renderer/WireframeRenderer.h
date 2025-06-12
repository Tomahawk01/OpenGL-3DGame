#pragma once

#include "Camera.h"
#include "LineData.h"
#include "Math/Vector3.h"
#include "Math/Color.h"

#include <vector>

namespace Game {

	class WireframeRenderer
	{
	public:
		void Draw(const vec3& start, const vec3& end, const Color& color);
		void Draw(const Camera& camera);

		std::vector<LineData> yield();

	private:
		std::vector<LineData> m_Lines;
	};

}
