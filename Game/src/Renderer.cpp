#include "Renderer.h"

#include "OpenGL.h"

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
		m_Mesh.Bind();
		::glDrawArrays(GL_TRIANGLES, 0, 3);
		m_Mesh.UnBind();
	}

}
