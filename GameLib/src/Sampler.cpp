#include "Sampler.h"

namespace Game {

	Sampler::Sampler()
		: m_Handle{ 0u, [](auto sampler) { ::glDeleteSamplers(1, &sampler); } }
	{
		::glCreateSamplers(1, &m_Handle);
	}

	::GLuint Sampler::GetNativeHandle() const
	{
		return m_Handle;
	}

}
