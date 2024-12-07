#include "Renderer.h"

#include "Camera.h"
#include "Entity.h"
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "OpenGL.h"

namespace Game {

	void Renderer::Render(const Camera& camera, const Scene& scene) const
	{
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const Entity* entity : scene.entities)
		{
			const Mesh* mesh = entity->GetMesh();
			const Material* material = entity->GetMaterial();

			::glUseProgram(material->GetNativeHandle());

			const GLint modelUniform = ::glGetUniformLocation(material->GetNativeHandle(), "model");
			::glUniformMatrix4fv(modelUniform, 1, GL_FALSE, entity->GetModel().data());

			const GLint viewUniform = ::glGetUniformLocation(material->GetNativeHandle(), "view");
			::glUniformMatrix4fv(viewUniform, 1, GL_FALSE, camera.GetView().data());

			const GLint projectionUniform = ::glGetUniformLocation(material->GetNativeHandle(), "projection");
			::glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, camera.GetProjection().data());

			mesh->Bind();
			::glDrawArrays(GL_TRIANGLES, 0, 36);
			mesh->UnBind();
		}
	}

}
