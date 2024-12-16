#include "Renderer.h"

#include "BufferWriter.h"
#include "Camera.h"
#include "Entity.h"
#include "Material.h"
#include "Texture.h"
#include "Sampler.h"
#include "Mesh.h"
#include "Scene.h"
#include "OpenGL.h"

namespace {

	struct LightBuffer
	{
		alignas(16) Game::Color ambient;
		alignas(16) Game::vec3 direction;
		alignas(16) Game::Color directionColor;
		alignas(16) Game::vec3 point;
		alignas(16) Game::Color pointColor;
		alignas(16) Game::vec3 attenuation;
	};

}

namespace Game {

	Renderer::Renderer()
		: m_CameraBuffer(sizeof(mat4) * 2u + sizeof(vec3))
		, m_LightBuffer(sizeof(LightBuffer))
	{}

	void Renderer::Render(const Camera& camera, const Scene& scene) const
	{
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
				.point = scene.pointLight.position,
				.pointColor = scene.pointLight.color,
				.attenuation = {
					scene.pointLight.constAttenuation,
					scene.pointLight.linearAttenuation,
					scene.pointLight.quadAttenuation }
			};
			BufferWriter writer{ m_LightBuffer };
			writer.Write(lightBuffer);
		}
		::glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_LightBuffer.GetNativeHandle());

		for (const Entity* entity : scene.entities)
		{
			const Mesh* mesh = entity->GetMesh();
			const Material* material = entity->GetMaterial();

			material->Use();
			material->SetUniform("model", entity->GetModel());
			material->BindTextures(entity->GetTextures());

			mesh->Bind();
			::glDrawElements(GL_TRIANGLES, mesh->IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh->IndexOffset()));
			mesh->UnBind();
		}
	}

}
