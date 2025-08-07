#include "Sampler.h"

namespace Game {

	Sampler::Sampler()
		: m_Handle{ 0u, [](auto sampler) { ::glDeleteSamplers(1, &sampler); } }
	{
		::glCreateSamplers(1, &m_Handle);

		::glSamplerParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		::glSamplerParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	::GLuint Sampler::GetNativeHandle() const
	{
		return m_Handle;
	}

}
