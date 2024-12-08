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

namespace Game {

	Renderer::Renderer()
		: m_CameraBuffer(sizeof(mat4) * 2u)
	{}

	void Renderer::Render(const Camera& camera, const Scene& scene) const
	{
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			BufferWriter writer{ m_CameraBuffer };
			writer.Write(camera.GetView());
			writer.Write(camera.GetProjection());
		}

		::glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CameraBuffer.GetNativeHandle());

		for (const Entity* entity : scene.entities)
		{
			const Mesh* mesh = entity->GetMesh();
			const Material* material = entity->GetMaterial();
			const Texture* texture = entity->GetTexture();
			const Sampler* sampler = entity->GetSampler();

			::glUseProgram(material->GetNativeHandle());

			const GLint modelUniform = ::glGetUniformLocation(material->GetNativeHandle(), "model");
			::glUniformMatrix4fv(modelUniform, 1, GL_FALSE, entity->GetModel().data());

			::glBindTextureUnit(0, texture->GetNativeHandle());
			::glBindSampler(0, sampler->GetNativeHandle());

			const GLint texUniform = ::glGetUniformLocation(material->GetNativeHandle(), "tex");
			::glUniform1i(texUniform, 0);

			mesh->Bind();
			::glDrawElements(GL_TRIANGLES, mesh->IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh->IndexOffset()));
			mesh->UnBind();
		}
	}

}
