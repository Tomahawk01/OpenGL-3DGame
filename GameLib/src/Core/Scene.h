#pragma once

#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Renderer/LineData.h"
#include "Renderer/Buffer.h"
#include "Physics/DebugLines.h"

#include <vector>
#include <optional>

namespace Game {

	class Entity;

	struct DirectionalLight
	{
		vec3 direction;
		Color color;
	};

	struct PointLight
	{
		vec3 position;
		Color color;
		float constAttenuation;
		float linearAttenuation;
		float quadAttenuation;
	};

	struct Scene
	{
		std::vector<const Entity*> entities;
		Color ambient;
		DirectionalLight directionalLight;
		std::vector<PointLight> pointLights;
		std::optional<DebugLines> debugLines;
	};

}
