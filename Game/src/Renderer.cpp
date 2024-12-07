#include "Renderer.h"

#include "BufferWriter.h"
#include "Camera.h"
#include "Entity.h"
#include "Material.h"
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

		::glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CameraBuffer.NativeHandle());

		for (const Entity* entity : scene.entities)
		{
			const Mesh* mesh = entity->GetMesh();
			const Material* material = entity->GetMaterial();

			::glUseProgram(material->GetNativeHandle());

			const GLint modelUniform = ::glGetUniformLocation(material->GetNativeHandle(), "model");
			::glUniformMatrix4fv(modelUniform, 1, GL_FALSE, entity->GetModel().data());

			mesh->Bind();
			::glDrawElements(GL_TRIANGLES, mesh->IndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(mesh->IndexOffset()));
			mesh->UnBind();
		}
	}

}
