#include "Sampler.h"

namespace Game {

	Sampler::Sampler()
		: m_Handle{ 0u, [](auto sampler) { ::glDeleteSamplers(1, &sampler); } }
	{
		::glCreateSamplers(1, &m_Handle);

		::glSamplerParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		::glSamplerParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		::glSamplerParameterf(m_Handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	}

	::GLuint Sampler::GetNativeHandle() const
	{
		return m_Handle;
	}

}
