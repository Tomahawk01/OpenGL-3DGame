#pragma once

#include "Core/Scene.h"
#include "Core/ResourceLoader.h"
#include "Renderer/Buffer.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/CubeMap.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Camera.h"
#include "MeshLoader.h"

namespace Game {

	class Renderer
	{
	public:
		Renderer(ResourceLoader& resourceLoader, MeshLoader& meshLoader, uint32_t width, uint32_t height);

		void Render(const Camera& camera, const Scene& scene, const CubeMap& skybox, const Sampler& skyboxSampler, float gamma) const;

	private:
		Buffer m_CameraBuffer;
		Buffer m_LightBuffer;
		Mesh m_SkyboxCube;
		Material m_SkyboxMaterial;
		Material m_DebugLineMaterial;
		FrameBuffer m_FB;
		Mesh m_PostProcessSprite;
		Material m_PostProcessMaterial;
	};

}
