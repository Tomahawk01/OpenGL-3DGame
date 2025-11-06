#include "Renderer.h"

#include "Core/Entity.h"
#include "BufferWriter.h"
#include "Texture.h"
#include "Sampler.h"
#include "OpenGL.h"

namespace {

	struct PointLightBuffer
	{
		alignas(16) Game::vec3 position;
		alignas(16) Game::Color color;
		alignas(16) Game::vec3 attenuation;
	};

	struct LightBuffer
	{
		alignas(16) Game::Color ambient;
		alignas(16) Game::vec3 direction;
		alignas(16) Game::Color directionColor;
		int numPoints;
	};

	Game::Material CreateMaterial(const Game::TLVReader& reader, std::string_view vertName, std::string_view fragName)
	{
		const Game::TextFile vertFile{ GetFile(reader, vertName) };
		const Game::TextFile fragFile{ GetFile(reader, fragName) };

		const Game::Shader vertexShader{ vertFile.Data, Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ fragFile.Data, Game::ShaderType::FRAGMENT };
		return Game::Material{ vertexShader, fragmentShader };
	}

	std::vector<Game::Texture> GenerateTextures(size_t n, Game::TextureUsage usage, uint32_t width, uint32_t height, uint8_t sampleCount)
	{
		std::vector<Game::Texture> textures{};

		std::ranges::generate_n(std::back_inserter(textures), n, [&]() { return Game::Texture{ usage, width, height, sampleCount }; });

		return textures;
	}

}

namespace Game {

	Renderer::Renderer(const TLVReader& reader, MeshLoader& meshLoader, uint32_t width, uint32_t height)
		: m_CameraBuffer(sizeof(mat4) * 2u + sizeof(vec3))
		, m_LightBuffer(10240u)
		, m_SkyboxCube(meshLoader.Cube())
		, m_SkyboxMaterial(CreateMaterial(reader, "cubeMap.vert", "cubeMap.frag"))
		, m_DebugLineMaterial(CreateMaterial(reader, "line.vert", "line.frag"))
		, m_MainFrameBuffer(
			GenerateTextures(3u, TextureUsage::FRAMEBUFFER, width, height, 8),
			{ TextureUsage::DEPTH, width, height, 8 })
		, m_PostProcessingFrameBuffer1(
			GenerateTextures(3u, TextureUsage::FRAMEBUFFER, width, height, 1),
			{ TextureUsage::DEPTH, width, height, 1 })
		, m_PostProcessingFrameBuffer2(
			GenerateTextures(3u, TextureUsage::FRAMEBUFFER, width, height, 1),
			{ TextureUsage::DEPTH, width, height, 1 })
		, m_SSAOApplyFrameBuffer(
			GenerateTextures(1u, TextureUsage::FRAMEBUFFER, width, height, 1),
			{ TextureUsage::DEPTH, width, height, 1 })
		, m_Sprite(meshLoader.Sprite())
		, m_HDRMaterial(CreateMaterial(reader, "hdr.vert", "hdr.frag"))
		, m_GreyScaleMaterial(CreateMaterial(reader, "greyScale.vert", "greyScale.frag"))
		, m_BlurMaterial(CreateMaterial(reader, "blur.vert", "blur.frag"))
		, m_LabelMaterial(CreateMaterial(reader, "label.vert", "label.frag"))
		, m_SSAOMaterial(CreateMaterial(reader, "ssao.vert", "ssao.frag"))
		, m_SSAOApplyMaterial(CreateMaterial(reader, "ssao.vert", "ssao_apply.frag"))
		, m_OrthCamera{ static_cast<float>(width), static_cast<float>(height), 1000u }
	{
		m_OrthCamera.SetPosition({ width / 2.0f, height / -2.0f, 0.0f });
	}

	void Renderer::Render(const Camera& camera, const Scene& scene, float gamma) const
	{
		m_MainFrameBuffer.frameBuffer.Bind();

		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			BufferWriter writer{ m_CameraBuffer };
			writer.Write(camera.GetView());
			writer.Write(camera.GetProjection());
			writer.Write(camera.GetPosition());
		}

		::glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CameraBuffer.GetNativeHandle());

		{
			LightBuffer lightBuffer{
				.ambient = scene.ambient,
				.direction = scene.directionalLight.direction,
				.directionColor = scene.directionalLight.color,
				.numPoints = static_cast<int>(scene.pointLights.size())
			};
			BufferWriter writer{ m_LightBuffer };
			writer.Write(lightBuffer);

			for (const auto& point : scene.pointLights)
			{
				PointLightBuffer pointLightBuffer{
					.position = point.position,
					.color = point.color,
					.attenuation = {
						point.constAttenuation, point.linearAttenuation, point.quadAttenuation
					}
				};

				writer.Write(pointLightBuffer);
			}
		}
		::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_LightBuffer.GetNativeHandle());

		for (const Entity* entity : scene.entities)
		{
			const Mesh* mesh = entity->GetMesh();
			const Material* material = entity->GetMaterial();

			material->Use();
			const mat4 model{ entity->GetTransform() };
			material->SetUniform("model", model);
			material->InvokeUniformCallback(entity);
			material->BindTextures(entity->GetTextures());

			mesh->Bind();
			::glDrawElements(GL_TRIANGLES, mesh->IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh->IndexOffset()));
			mesh->UnBind();
		}

		if (const auto& dbl = scene.debugLines; dbl)
		{
			m_DebugLineMaterial.Use();
			dbl->Bind();
			::glDrawArrays(GL_LINES, 0u, dbl->Count());
			dbl->UnBind();
		}

		m_MainFrameBuffer.frameBuffer.UnBind();

		for (::GLenum i = 0; i < 3; ++i)
		{
			::glNamedFramebufferReadBuffer(m_MainFrameBuffer.frameBuffer.GetNativeHandle(), GL_COLOR_ATTACHMENT0 + i);
			::glNamedFramebufferDrawBuffer(m_PostProcessingFrameBuffer1.frameBuffer.GetNativeHandle(), GL_COLOR_ATTACHMENT0 + i);
			::glBlitNamedFramebuffer(
				m_MainFrameBuffer.frameBuffer.GetNativeHandle(),
				m_PostProcessingFrameBuffer1.frameBuffer.GetNativeHandle(),
				0u,
				0u,
				m_MainFrameBuffer.frameBuffer.GetWidth(),
				m_MainFrameBuffer.frameBuffer.GetHeight(),
				0u,
				0u,
				m_PostProcessingFrameBuffer1.frameBuffer.GetWidth(),
				m_PostProcessingFrameBuffer1.frameBuffer.GetHeight(),
				GL_COLOR_BUFFER_BIT,
				GL_NEAREST);
		}

		::glNamedFramebufferReadBuffer(m_MainFrameBuffer.frameBuffer.GetNativeHandle(), GL_COLOR_ATTACHMENT0);
		::glNamedFramebufferDrawBuffer(m_PostProcessingFrameBuffer1.frameBuffer.GetNativeHandle(), GL_COLOR_ATTACHMENT0);
		::glBlitNamedFramebuffer(
			m_MainFrameBuffer.frameBuffer.GetNativeHandle(),
			m_PostProcessingFrameBuffer1.frameBuffer.GetNativeHandle(),
			0u,
			0u,
			m_MainFrameBuffer.frameBuffer.GetWidth(),
			m_MainFrameBuffer.frameBuffer.GetHeight(),
			0u,
			0u,
			m_PostProcessingFrameBuffer1.frameBuffer.GetWidth(),
			m_PostProcessingFrameBuffer1.frameBuffer.GetHeight(),
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST);

		auto* readFB = &m_PostProcessingFrameBuffer1.frameBuffer;
		auto* writeFB = &m_PostProcessingFrameBuffer2.frameBuffer;
		
		if (scene.effects.ssao)
		{
			readFB->UnBind();
			writeFB->Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_SSAOMaterial.Use();
			m_Sprite.Bind();
			m_SSAOMaterial.BindTexture(0, &m_PostProcessingFrameBuffer1.colorTextures[1], scene.skyboxSampler);
			m_SSAOMaterial.BindTexture(1, &m_PostProcessingFrameBuffer1.colorTextures[2], scene.skyboxSampler);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));
			m_Sprite.UnBind();

			writeFB->UnBind();
			m_SSAOApplyFrameBuffer.frameBuffer.Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_SSAOApplyMaterial.Use();
			m_Sprite.Bind();
			m_SSAOApplyMaterial.BindTexture(0, &m_PostProcessingFrameBuffer1.colorTextures[0], scene.skyboxSampler);
			m_SSAOApplyMaterial.BindTexture(1, &m_PostProcessingFrameBuffer2.colorTextures[0], scene.skyboxSampler);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));
			m_Sprite.UnBind();

			::glBlitNamedFramebuffer(
				m_SSAOApplyFrameBuffer.frameBuffer.GetNativeHandle(),
				readFB->GetNativeHandle(),
				0u,
				0u,
				m_SSAOApplyFrameBuffer.frameBuffer.GetWidth(),
				m_SSAOApplyFrameBuffer.frameBuffer.GetHeight(),
				0u,
				0u,
				readFB->GetWidth(),
				readFB->GetHeight(),
				GL_COLOR_BUFFER_BIT,
				GL_NEAREST);
			m_SSAOApplyFrameBuffer.frameBuffer.UnBind();
		}

		if (scene.skybox)
		{
			::glDepthFunc(GL_LEQUAL);
			readFB->Bind();

			m_SkyboxMaterial.Use();
			m_SkyboxCube.Bind();

			m_SkyboxMaterial.BindCubeMap(scene.skybox, scene.skyboxSampler);
			::glDrawElements(GL_TRIANGLES, m_SkyboxCube.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_SkyboxCube.IndexOffset()));

			m_SkyboxCube.UnBind();
			::glDepthFunc(GL_LESS);
		}

		if (scene.effects.hdr)
		{
			readFB->UnBind();
			writeFB->Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			m_HDRMaterial.Use();
			m_Sprite.Bind();
			m_HDRMaterial.BindTexture(0, readFB->GetColorTextures().front(), scene.skyboxSampler);
			m_HDRMaterial.SetUniform("gamma", gamma);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));
			m_Sprite.UnBind();
		
			std::ranges::swap(readFB, writeFB);
		}
		
		if (scene.effects.grayScale)
		{
			readFB->UnBind();
			writeFB->Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			m_GreyScaleMaterial.Use();
			m_Sprite.Bind();
			m_GreyScaleMaterial.BindTexture(0, readFB->GetColorTextures().front(), scene.skyboxSampler);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));
			m_Sprite.UnBind();
		
			std::ranges::swap(readFB, writeFB);
		}
		
		if (scene.effects.blur)
		{
			readFB->UnBind();
			writeFB->Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			m_BlurMaterial.Use();
			m_Sprite.Bind();
			m_BlurMaterial.BindTexture(0, readFB->GetColorTextures().front(), scene.skyboxSampler);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));
			m_Sprite.UnBind();
		
			std::ranges::swap(readFB, writeFB);
		}
		
		// NOTE: Render UI
		m_LabelMaterial.Use();
		m_Sprite.Bind();
		for (const auto& [texture, x, y] : scene.labels)
		{
			{
				BufferWriter writer{ m_CameraBuffer };
				writer.Write(m_OrthCamera.GetView());
				writer.Write(m_OrthCamera.GetProjection());
				writer.Write(m_OrthCamera.GetPosition());
			}
			::glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CameraBuffer.GetNativeHandle());
		
			const mat4 model{ 
				vec3{static_cast<float>(x) + (texture->GetWidth() / 2.0f), -static_cast<float>(y) - (texture->GetHeight() / 2.0f), 0.0f},
				vec3{static_cast<float>(texture->GetWidth()) / 2.0f, static_cast<float>(texture->GetHeight()) / 2.0f, 1.0f}
			};
			m_LabelMaterial.SetUniform("model", model);
			m_LabelMaterial.BindTexture(0, texture);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));
		}
		m_Sprite.UnBind();

		::glBlitNamedFramebuffer(
			readFB->GetNativeHandle(),
			0u,
			0u,
			0u,
			readFB->GetWidth(),
			readFB->GetHeight(),
			0u,
			0u,
			readFB->GetWidth(),
			readFB->GetHeight(),
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST);
	}

}
