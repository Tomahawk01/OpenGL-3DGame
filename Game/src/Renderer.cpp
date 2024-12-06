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

	void Renderer::Render() const
	{
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		::glUseProgram(m_Material.GetNativeHandle());

		static float x = 2.0f;
		static float z = 0.0f;
		static float t = 0.0f;

		x = std::sin(t) * 5.0f;
		z = std::cos(t) * 5.0f;

		t += 0.01f;

		static constexpr mat4 model{ vec3{0.0f, 0.0f, 0.0f} };
		const GLint modelUniform = ::glGetUniformLocation(m_Material.GetNativeHandle(), "model");
		::glUniformMatrix4fv(modelUniform, 1, GL_FALSE, model.data().data());

		const mat4 view{ mat4::LookAt({x, 0.0f, z}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f})};
		const GLint viewUniform = ::glGetUniformLocation(m_Material.GetNativeHandle(), "view");
		::glUniformMatrix4fv(viewUniform, 1, GL_FALSE, view.data().data());

		static const mat4 projection{ mat4::Perspective(std::numbers::pi_v<float> / 4.0f, 800.0f, 600.0f, 0.1f, 100.0f) };
		const GLint projectionUniform = ::glGetUniformLocation(m_Material.GetNativeHandle(), "projection");
		::glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, projection.data().data());

		m_Mesh.Bind();
		::glDrawArrays(GL_TRIANGLES, 0, 36);
		m_Mesh.UnBind();
	}

}
