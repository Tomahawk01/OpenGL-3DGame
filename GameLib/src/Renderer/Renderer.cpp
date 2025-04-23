#include "Renderer.h"

#include "Core/Entity.h"
#include "Core/Scene.h"
#include "BufferWriter.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"
#include "Sampler.h"
#include "Mesh.h"
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

	Game::Material CreateSkyboxMaterial(Game::ResourceLoader& resourceLoader)
	{
		const Game::File vertFile{ resourceLoader.Load("shaders/cubeMap.vert") };
		const Game::File fragFile{ resourceLoader.Load("shaders/cubeMap.frag") };

		const Game::Shader vertexShader{ vertFile.AsString(), Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ fragFile.AsString(), Game::ShaderType::FRAGMENT };
		return Game::Material{ vertexShader, fragmentShader };
	}

	Game::Material CreateDebugLineMaterial(Game::ResourceLoader& resourceLoader)
	{
		const Game::File vertFile{ resourceLoader.Load("shaders/line.vert") };
		const Game::File fragFile{ resourceLoader.Load("shaders/line.frag") };

		const Game::Shader vertexShader{ vertFile.AsString(), Game::ShaderType::VERTEX};
		const Game::Shader fragmentShader{ fragFile.AsString(), Game::ShaderType::FRAGMENT};
		return Game::Material{ vertexShader, fragmentShader };
	}

	Game::Material CreatePostProcessMaterial(Game::ResourceLoader& resourceLoader)
	{
		const Game::File vertFile{ resourceLoader.Load("shaders/postProcess.vert") };
		const Game::File fragFile{ resourceLoader.Load("shaders/postProcess.frag") };

		const Game::Shader vertexShader{ vertFile.AsString(), Game::ShaderType::VERTEX };
		const Game::Shader fragmentShader{ fragFile.AsString(), Game::ShaderType::FRAGMENT };
		return Game::Material{ vertexShader, fragmentShader };
	}

}

namespace Game {

	Renderer::Renderer(ResourceLoader& resourceLoader, MeshLoader& meshLoader, uint32_t width, uint32_t height)
		: m_CameraBuffer(sizeof(mat4) * 2u + sizeof(vec3))
		, m_LightBuffer(10240u)
		, m_SkyboxCube(meshLoader.Cube())
		, m_SkyboxMaterial(CreateSkyboxMaterial(resourceLoader))
		, m_DebugLineMaterial(CreateDebugLineMaterial(resourceLoader))
		, m_FB(width, height)
		, m_PostProcessSprite(meshLoader.Sprite())
		, m_PostProcessMaterial(CreatePostProcessMaterial(resourceLoader))
	{}

	void Renderer::Render(const Camera& camera, const Scene& scene, float gamma) const
	{
		m_FB.Bind();

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

		for (const Entity* entity : scene.entities)
		{
			const Mesh* mesh = entity->GetMesh();
			const Material* material = entity->GetMaterial();

			material->Use();
			const mat4 model{ entity->GetTransform() };
			material->SetUniform("model", model);
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

		m_FB.UnBind();

		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_PostProcessMaterial.Use();
		m_PostProcessSprite.Bind();
		m_PostProcessMaterial.BindTexture(0, &m_FB.GetColorTexture(), scene.skyboxSampler);
		m_PostProcessMaterial.SetUniform("gamma", gamma);
		::glDrawElements(GL_TRIANGLES, m_PostProcessSprite.IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(m_PostProcessSprite.IndexOffset()));
		m_PostProcessSprite.UnBind();
	}

}
