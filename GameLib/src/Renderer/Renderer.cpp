#include "Renderer.h"

#include "Core/Entity.h"
#include "BufferWriter.h"
#include "Texture.h"
#include "Sampler.h"
#include "OpenGL.h"

namespace {

	template<class... T>
	class AutoBind
	{
	public:
		AutoBind(T&... obj)
			: m_Objs{ obj... }
		{
			[this]<size_t... Ix>(std::index_sequence<Ix...>)
			{
				(std::get<Ix>(this->m_Objs).Bind(), ...);
			}(std::make_index_sequence<sizeof...(T)>());
		}

		~AutoBind()
		{
			[this]<size_t... Ix>(std::index_sequence<Ix...>)
			{
				(std::get<Ix>(this->m_Objs).UnBind(), ...);
			}(std::make_index_sequence<sizeof...(T)>());
		}

	private:
		std::tuple<T&...> m_Objs;
	};

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

	void Blit(const Game::FrameBuffer& sourceFB, ::GLenum sourceAttachment, const Game::FrameBuffer& destFB, ::GLenum destAttachment, ::GLbitfield mask = GL_COLOR_BUFFER_BIT)
	{
		::glNamedFramebufferReadBuffer(sourceFB.GetNativeHandle(), sourceAttachment);
		::glNamedFramebufferDrawBuffer(destFB.GetNativeHandle(), destAttachment);
		::glBlitNamedFramebuffer(
			sourceFB.GetNativeHandle(),
			destFB.GetNativeHandle(),
			0u,
			0u,
			sourceFB.GetWidth(),
			sourceFB.GetHeight(),
			0u,
			0u,
			destFB.GetWidth(),
			destFB.GetHeight(),
			mask,
			GL_NEAREST);
	}

	void ApplyPostProccesingEffect(Game::FrameBuffer const*& readFB, Game::FrameBuffer const*& writeFB, const Game::Material& material, const Game::Sampler* sampler, const Game::Mesh& sprite, float gamma = -1)
	{
		auto& fb = *writeFB;
		AutoBind bind{ fb };
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		material.Use();
		material.BindTexture(0, readFB->GetColorTextures().front(), sampler);
		if (gamma != -1)
		{
			material.SetUniform("gamma", gamma);
		}
		::glDrawElements(GL_TRIANGLES, sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(sprite.IndexOffset()));

		std::ranges::swap(readFB, writeFB);
	}

	void WriteCameraDataToUBO(const Game::Camera& camera, const Game::Buffer& cameraBuffer)
	{
		Game::BufferWriter writer{ cameraBuffer };
		writer.Write(camera.GetView());
		writer.Write(camera.GetProjection());
		writer.Write(camera.GetPosition());

		::glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraBuffer.GetNativeHandle());
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

		WriteCameraDataToUBO(camera, m_CameraBuffer);

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

			const AutoBind bind{ *mesh };
			::glDrawElements(GL_TRIANGLES, mesh->IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh->IndexOffset()));
		}

		if (const auto& dbl = scene.debugLines; dbl)
		{
			m_DebugLineMaterial.Use();
			const AutoBind bind{ *dbl };
			::glDrawArrays(GL_LINES, 0u, dbl->Count());
		}

		for (::GLenum i = 0; i < 3; ++i)
		{
			Blit(m_MainFrameBuffer.frameBuffer, GL_COLOR_ATTACHMENT0 + i, m_PostProcessingFrameBuffer1.frameBuffer, GL_COLOR_ATTACHMENT0 + i);
		}

		Blit(m_MainFrameBuffer.frameBuffer, GL_COLOR_ATTACHMENT0, m_PostProcessingFrameBuffer1.frameBuffer, GL_COLOR_ATTACHMENT0, GL_DEPTH_BUFFER_BIT);

		auto* readFB = &m_PostProcessingFrameBuffer1.frameBuffer;
		auto* writeFB = &m_PostProcessingFrameBuffer2.frameBuffer;

		if (scene.effects.ssao)
		{
			const AutoBind bind{ m_Sprite };

			writeFB->Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_SSAOMaterial.Use();
			m_SSAOMaterial.BindTexture(0, &m_PostProcessingFrameBuffer1.colorTextures[1], scene.skyboxSampler);
			m_SSAOMaterial.BindTexture(1, &m_PostProcessingFrameBuffer1.colorTextures[2], scene.skyboxSampler);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));

			m_SSAOApplyFrameBuffer.frameBuffer.Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_SSAOApplyMaterial.Use();
			m_SSAOApplyMaterial.BindTexture(0, &m_PostProcessingFrameBuffer1.colorTextures[0], scene.skyboxSampler);
			m_SSAOApplyMaterial.BindTexture(1, &m_PostProcessingFrameBuffer2.colorTextures[0], scene.skyboxSampler);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));

			Blit(m_SSAOApplyFrameBuffer.frameBuffer, GL_COLOR_ATTACHMENT0, *readFB, GL_COLOR_ATTACHMENT0);
		}

		if (scene.skybox)
		{
			::glDepthFunc(GL_LEQUAL);
			const AutoBind bind{ *readFB, m_SkyboxCube };

			m_SkyboxMaterial.Use();
			m_SkyboxMaterial.BindCubeMap(scene.skybox, scene.skyboxSampler);
			::glDrawElements(GL_TRIANGLES, m_SkyboxCube.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_SkyboxCube.IndexOffset()));

			::glDepthFunc(GL_LESS);
		}

		{
			const AutoBind bind{ m_Sprite };

			if (scene.effects.hdr)
			{
				ApplyPostProccesingEffect(readFB, writeFB, m_HDRMaterial, scene.skyboxSampler, m_Sprite, gamma);
			}

			if (scene.effects.grayScale)
			{
				ApplyPostProccesingEffect(readFB, writeFB, m_GreyScaleMaterial, scene.skyboxSampler, m_Sprite);
			}

			if (scene.effects.blur)
			{
				ApplyPostProccesingEffect(readFB, writeFB, m_BlurMaterial, scene.skyboxSampler, m_Sprite);
			}

			// NOTE: Render UI
			m_LabelMaterial.Use();
			const AutoBind bind2{ *readFB };

			for (const auto& [texture, x, y] : scene.labels)
			{
				WriteCameraDataToUBO(m_OrthCamera, m_CameraBuffer);
				const mat4 model{
					vec3{static_cast<float>(x) + (texture->GetWidth() / 2.0f), -static_cast<float>(y) - (texture->GetHeight() / 2.0f), 0.0f},
					vec3{static_cast<float>(texture->GetWidth()) / 2.0f, static_cast<float>(texture->GetHeight()) / 2.0f, 1.0f}
				};
				m_LabelMaterial.SetUniform("model", model);
				m_LabelMaterial.BindTexture(0, texture);
				::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));
			}
		}

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
