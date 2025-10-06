#pragma once

#include "TLV/Utilities.h"
#include "Core/Scene.h"
#include "Buffer.h"
#include "FrameBuffer.h"
#include "CubeMap.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"
#include "MeshLoader.h"

#include <vector>
#include <ranges>
#include <span>
#include <memory>

namespace Game {

	class Renderer
	{
	public:
		Renderer(const TLVReader& reader, MeshLoader& meshLoader, uint32_t width, uint32_t height);

		void Render(const Camera& camera, const Scene& scene, float gamma) const;

	private:

		struct FrameBufferData
		{
			FrameBufferData(std::vector<Texture> colorTextures, Texture depth)
				: colorTextures{ std::move(colorTextures) }
				, colorTexturePointers{ this->colorTextures |
					std::views::transform([](const auto& e) -> const Texture* { return std::addressof(e); }) |
					std::ranges::to<std::vector>() }
				, depth{ std::move(depth) }
				, frameBuffer{ colorTexturePointers, &depth }
			{}

			std::vector<Texture> colorTextures;
			std::vector<const Texture*> colorTexturePointers;
			Texture depth;
			FrameBuffer frameBuffer;
		};

		Buffer m_CameraBuffer;
		Buffer m_LightBuffer;
		Mesh m_SkyboxCube;
		Material m_SkyboxMaterial;
		Material m_DebugLineMaterial;
		FrameBufferData m_MainFrameBuffer;
		FrameBufferData m_PostProcessingFrameBuffer1;
		FrameBufferData m_PostProcessingFrameBuffer2;
		Mesh m_Sprite;
		Material m_HDRMaterial;
		Material m_GreyScaleMaterial;
		Material m_BlurMaterial;
		Material m_LabelMaterial;
		Camera m_OrthCamera;
	};

}
