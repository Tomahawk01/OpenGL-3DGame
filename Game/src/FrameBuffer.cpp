#include "FrameBuffer.h"

namespace Game {

	FrameBuffer::FrameBuffer(std::uint32_t width, std::uint32_t height)
		: m_Handle(0u, [](const auto buffer) { ::glDeleteFramebuffers(1u, &buffer); })
		, m_Width(width)
		, m_Height(height)
		, m_ColorTexture(TextureUsage::FRAMEBUFFER, width, height)
		, m_DepthTexture(TextureUsage::DEPTH, width, height)
	{
		::glCreateFramebuffers(1, &m_Handle);
		::glNamedFramebufferTexture(m_Handle, GL_COLOR_ATTACHMENT0, m_ColorTexture.GetNativeHandle(), 0);
		::glNamedFramebufferTexture(m_Handle, GL_DEPTH_ATTACHMENT, m_DepthTexture.GetNativeHandle(), 0);
	}

	void FrameBuffer::Bind() const
	{
		::glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
	}

	void FrameBuffer::UnBind() const
	{
		::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
	}

	std::uint32_t FrameBuffer::GetWidth() const
	{
		return m_Width;
	}

	std::uint32_t FrameBuffer::GetHeight() const
	{
		return m_Height;
	}

	::GLuint FrameBuffer::GetNativeHandle() const
	{
		return m_Handle;
	}

}
