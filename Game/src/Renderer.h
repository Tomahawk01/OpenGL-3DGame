#pragma once

#include "Material.h"
#include "Mesh.h"

namespace Game {

	class Renderer
	{
	public:
		Renderer(Material material);

		void Render() const;

	private:
		Mesh m_Mesh;
		Material m_Material;
	};

}
