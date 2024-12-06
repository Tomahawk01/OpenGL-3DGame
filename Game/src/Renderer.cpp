#include "Renderer.h"

#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "OpenGL.h"

#include <numbers>

namespace Game {

	Renderer::Renderer(Material material)
		: m_Mesh{}
		, m_Material(std::move(material))
	{
		::glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	}

	void Renderer::Render() const
	{
		::glClear(GL_COLOR_BUFFER_BIT);

		::glUseProgram(m_Material.NativeHandle());

		static constexpr mat4 model{ vec3{0.0f, 0.0f, 0.0f} };
		const GLint modelUniform = ::glGetUniformLocation(m_Material.NativeHandle(), "model");
		::glUniformMatrix4fv(modelUniform, 1, GL_FALSE, model.data().data());

		static const mat4 view{ mat4::LookAt({1.0f, 0.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f})};
		const GLint viewUniform = ::glGetUniformLocation(m_Material.NativeHandle(), "view");
		::glUniformMatrix4fv(viewUniform, 1, GL_FALSE, view.data().data());

		static const mat4 projection{ mat4::Perspective(std::numbers::pi_v<float> / 4.0f, 800.0f, 600.0f, 0.001f, 100.0f) };
		const GLint projectionUniform = ::glGetUniformLocation(m_Material.NativeHandle(), "projection");
		::glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, projection.data().data());

		m_Mesh.Bind();
		::glDrawArrays(GL_TRIANGLES, 0, 3);
		m_Mesh.UnBind();
	}

}
