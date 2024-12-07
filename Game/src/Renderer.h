#pragma once

#include "Camera.h"
#include "Scene.h"

namespace Game {

	class Renderer
	{
	public:
		void Render(const Camera& camera, const Scene& scene) const;

	private:
	};

}
