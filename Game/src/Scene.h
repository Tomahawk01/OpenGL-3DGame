#pragma once

#include <vector>

namespace Game {

	class Entity;

	struct Scene
	{
		std::vector<const Entity*> entities;
	};

}
