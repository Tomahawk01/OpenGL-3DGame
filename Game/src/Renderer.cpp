#include "Renderer.h"

#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "OpenGL.h"

#include <cmath>
#include <numbers>

namespace Game {

	Renderer::Renderer(Material material)
		: m_Mesh{}
		, m_Material(std::move(material))
	{
		::glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	}

	void Renderer::Render(const Camera& camera) const
	{
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		::glUseProgram(m_Material.GetNativeHandle());

		static constexpr mat4 model{ vec3{0.0f, 0.0f, 0.0f} };
		const GLint modelUniform = ::glGetUniformLocation(m_Material.GetNativeHandle(), "model");
		::glUniformMatrix4fv(modelUniform, 1, GL_FALSE, model.data().data());

		const GLint viewUniform = ::glGetUniformLocation(m_Material.GetNativeHandle(), "view");
		::glUniformMatrix4fv(viewUniform, 1, GL_FALSE, camera.GetView().data());

		const GLint projectionUniform = ::glGetUniformLocation(m_Material.GetNativeHandle(), "projection");
		::glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, camera.GetProjection().data());

		m_Mesh.Bind();
		::glDrawArrays(GL_TRIANGLES, 0, 36);
		m_Mesh.UnBind();
	}

}
