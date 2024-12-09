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
				.pointColor = scene.pointLight.color
			};
			BufferWriter writer{ m_LightBuffer };
			writer.Write(lightBuffer);
		}
		::glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_LightBuffer.GetNativeHandle());

		for (const Entity* entity : scene.entities)
		{
			const Mesh* mesh = entity->GetMesh();
			const Material* material = entity->GetMaterial();
			const Sampler* sampler = entity->GetSampler();

			::glUseProgram(material->GetNativeHandle());

			const GLint modelUniform = ::glGetUniformLocation(material->GetNativeHandle(), "model");
			::glUniformMatrix4fv(modelUniform, 1, GL_FALSE, entity->GetModel().data());

			for (const auto& [index, tex] : entity->GetTextures() | std::views::enumerate)
			{
				const ::GLuint glIndex = static_cast<::GLuint>(index);
				::glBindTextureUnit(glIndex, tex->GetNativeHandle());
				::glBindSampler(glIndex, sampler->GetNativeHandle());

				const std::string uniformName = std::format("tex{}", glIndex);

				const GLint texUniform = ::glGetUniformLocation(material->GetNativeHandle(), uniformName.c_str());
				::glUniform1i(texUniform, glIndex);
			}

			mesh->Bind();
			::glDrawElements(GL_TRIANGLES, mesh->IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh->IndexOffset()));
			mesh->UnBind();
		}
	}

}
