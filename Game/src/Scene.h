#pragma once

#include "Math/Color.h"
#include "Math/Vector3.h"

#include <vector>

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
		PointLight pointLight;
	};

}
