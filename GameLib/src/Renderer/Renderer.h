#pragma once

#include "TLV/Utilities.h"
#include "Core/Scene.h"
#include "Core/ResourceLoader.h"
#include "Buffer.h"
#include "FrameBuffer.h"
#include "CubeMap.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "MeshLoader.h"

namespace Game {

	class Renderer
	{
	public:
		Renderer(const TLVReader& reader, MeshLoader& meshLoader, uint32_t width, uint32_t height);

		void Render(const Camera& camera, const Scene& scene, float gamma) const;

	private:
		Buffer m_CameraBuffer;
		Buffer m_LightBuffer;
		Mesh m_SkyboxCube;
		Material m_SkyboxMaterial;
		Material m_DebugLineMaterial;
		FrameBuffer m_FB;
		Mesh m_PostProcessSprite;
		Material m_PostProcessMaterial;
		Material m_LabelMaterial;
	};

}
