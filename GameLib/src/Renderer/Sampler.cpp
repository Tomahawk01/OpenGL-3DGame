#include "Sampler.h"

#include "Utilities/Error.h"
#include "Utilities/Exception.h"
#include "Utilities/Formatter.h"

namespace {

	::GLenum ToOpenGL(Game::FilterType filterType)
	{
		switch (filterType)
		{
			case Game::FilterType::LINEAR_MIPMAP: return GL_LINEAR_MIPMAP_LINEAR;
			case Game::FilterType::LINEAR: return GL_LINEAR;
			case Game::FilterType::NEAREST: return GL_NEAREST;
		}

		throw Game::Exception("Unknown filerType: {}", filterType);
	}

}

namespace Game {

	Sampler::Sampler(FilterType minFilter, FilterType magFilter, std::optional<float> anisotropySamples)
		: m_Handle{ 0u, [](auto sampler) { ::glDeleteSamplers(1, &sampler); } }
	{
		::glCreateSamplers(1, &m_Handle);

		::glSamplerParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, ToOpenGL(minFilter));
		::glSamplerParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, ToOpenGL(magFilter));

		if (anisotropySamples)
		{
			Expect(*anisotropySamples >= 1.0f, "Invalid samples: {}", *anisotropySamples);
			::glSamplerParameterf(m_Handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, *anisotropySamples);
		}
	}

	::GLuint Sampler::GetNativeHandle() const
	{
		return m_Handle;
	}

}
