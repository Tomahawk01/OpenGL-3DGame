#pragma once

#include "Camera.h"
#include "Scene.h"
#include "Buffer.h"
#include "FrameBuffer.h"
#include "CubeMap.h"
#include "Material.h"
#include "Mesh.h"
#include "ResourceLoader.h"
#include "MeshLoader.h"

namespace Game {

	class Renderer
	{
	public:
		Renderer(ResourceLoader& resourceLoader, MeshLoader& meshLoader, std::uint32_t width, std::uint32_t height);

		void Render(const Camera& camera, const Scene& scene, const CubeMap& skybox, const Sampler& skyboxSampler) const;

	private:
		Buffer m_CameraBuffer;
		Buffer m_LightBuffer;
		Mesh m_SkyboxCube;
		Material m_SkyboxMaterial;
		FrameBuffer m_FB;
	};

}
