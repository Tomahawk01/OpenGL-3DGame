#pragma once

#include "Camera.h"
#include "Material.h"
#include "Mesh.h"

namespace Game {

	class Renderer
	{
	public:
		Renderer(Material material);

		void Render(const Camera& camera) const;

	private:
		Mesh m_Mesh;
		Material m_Material;
	};

}
