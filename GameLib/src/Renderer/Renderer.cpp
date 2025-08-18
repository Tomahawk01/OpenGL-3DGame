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

}

namespace Game {

	Renderer::Renderer(const TLVReader& reader, MeshLoader& meshLoader, uint32_t width, uint32_t height)
		: m_CameraBuffer(sizeof(mat4) * 2u + sizeof(vec3))
		, m_LightBuffer(10240u)
		, m_SkyboxCube(meshLoader.Cube())
		, m_SkyboxMaterial(CreateMaterial(reader, "cubeMap.vert", "cubeMap.frag"))
		, m_DebugLineMaterial(CreateMaterial(reader, "line.vert", "line.frag"))
		, m_FB1(width, height)
		, m_FB2(width, height)
		, m_Sprite(meshLoader.Sprite())
		, m_HDRMaterial(CreateMaterial(reader, "hdr.vert", "hdr.frag"))
		, m_GreyScaleMaterial(CreateMaterial(reader, "greyScale.vert", "greyScale.frag"))
		, m_LabelMaterial(CreateMaterial(reader, "label.vert", "label.frag"))
		, m_OrthCamera{ static_cast<float>(width), static_cast<float>(height), 1000u }
	{
		m_OrthCamera.SetPosition({ width / 2.0f, height / -2.0f, 0.0f });
	}

	void Renderer::Render(const Camera& camera, const Scene& scene, float gamma) const
	{
		m_FB1.Bind();

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

		::glDepthMask(GL_FALSE);

		m_SkyboxMaterial.Use();
		m_SkyboxCube.Bind();

		m_SkyboxMaterial.BindCubeMap(scene.skybox, scene.skyboxSampler);
		::glDrawElements(GL_TRIANGLES, m_SkyboxCube.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_SkyboxCube.IndexOffset()));

		m_SkyboxCube.UnBind();

		::glDepthMask(GL_TRUE);

		for (const Entity* entity : scene.entities)// | std::views::filter([](const auto* e) { return e->IsVisible(); }))
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

		m_FB1.UnBind();

		auto* readFB = &m_FB1;
		auto* writeFB = &m_FB2;

		if (scene.effects.hdr)
		{
			writeFB->Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_HDRMaterial.Use();
			m_Sprite.Bind();
			m_HDRMaterial.BindTexture(0, &readFB->GetColorTexture(), scene.skyboxSampler);
			m_HDRMaterial.SetUniform("gamma", gamma);
			::glDrawElements(GL_TRIANGLES, m_Sprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_Sprite.IndexOffset()));
			m_Sprite.UnBind();

			std::ranges::swap(readFB, writeFB);
		}

		if (scene.effects.grayScale)
		{
			writeFB->Bind();
			::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_GreyScaleMaterial.Use();
			m_Sprite.Bind();
			m_GreyScaleMaterial.BindTexture(0, &readFB->GetColorTexture(), scene.skyboxSampler);
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
				vec3{static_cast<float>(x) + (texture->GetWidth() / 2.0f), static_cast<float>(y) - (texture->GetHeight() / 2.0f), 0.0f},
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
			m_FB1.GetWidth(),
			m_FB1.GetHeight(),
			0u,
			0u,
			m_FB1.GetWidth(),
			m_FB1.GetHeight(),
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST);
	}

}
