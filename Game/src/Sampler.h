#pragma once

#include "Utilities/AutoRelease.h"
#include "OpenGL.h"

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
