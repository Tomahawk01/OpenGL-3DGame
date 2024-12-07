#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Buffer.h"

namespace Game {

	class Renderer
	{
	public:
		Renderer();

		void Render(const Camera& camera, const Scene& scene) const;

	private:
		Buffer m_CameraBuffer;
	};

}
