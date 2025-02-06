#pragma once

#include "Utilities/AutoRelease.h"
#include "Renderer/OpenGL.h"

namespace Game {

	class Sampler
	{
	public:
		Sampler();

		::GLuint GetNativeHandle() const;

	private:
		AutoRelease<::GLuint> m_Handle;
	};

}
