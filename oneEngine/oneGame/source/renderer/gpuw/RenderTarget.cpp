#include "renderer/gpuw/RenderTarget.h"
#include "renderer/gpuw/Error.h"
#include "renderer/gpuw/ogl/GLCommon.h"
#include "core/debug.h"

gpu::RenderTarget::RenderTarget ( void )
	:
	m_framebuffer(0), m_attachments(),
	m_assembled(false)
{}

gpu::RenderTarget::~RenderTarget ( void )
{}

int gpu::RenderTarget::create ( Device* device )
{
	glCreateFramebuffers(1, &m_framebuffer);
	return 0;
}

int gpu::RenderTarget::destroy ( Device* device )
{
	if (m_framebuffer != 0)
	{
		glDeleteFramebuffers(1, &m_framebuffer);
		m_framebuffer = 0;
	}
	return 0;
}

int gpu::RenderTarget::attach ( int slot, Texture* texture )
{
	if (slot == kRenderTargetSlotDepth)
	{
		glNamedFramebufferTexture(m_framebuffer, GL_DEPTH_ATTACHMENT, (GLuint)texture->nativePtr(), 0);
		m_attachmentDepth = texture;
	}
	else if (slot == kRenderTargetSlotStencil)
	{
		glNamedFramebufferTexture(m_framebuffer, GL_STENCIL_ATTACHMENT, (GLuint)texture->nativePtr(), 0);
		m_attachmentStencil = texture;
	}
	else if (slot >= kRenderTargetSlotColor0)
	{
		glNamedFramebufferTexture(m_framebuffer, GL_COLOR_ATTACHMENT0 + (slot - kRenderTargetSlotColor0), (GLuint)texture->nativePtr(), 0);
		m_attachments[slot] = texture;
		m_attachmentIsTexture[slot] = true;
	}
	else
	{
		return kErrorBadArgument;
	}

	return 0;
}

int gpu::RenderTarget::attach ( int slot, WOFrameAttachment* buffer )
{
	if (slot == kRenderTargetSlotDepth)
	{
		glNamedFramebufferRenderbuffer(m_framebuffer, GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, (GLuint)buffer->nativePtr());
		m_attachmentDepth = buffer;
	}
	else if (slot == kRenderTargetSlotStencil)
	{
		glNamedFramebufferRenderbuffer(m_framebuffer, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, (GLuint)buffer->nativePtr());
		m_attachmentStencil = buffer;
	}
	else if (slot >= kRenderTargetSlotColor0)
	{
		glNamedFramebufferRenderbuffer(m_framebuffer, GL_COLOR_ATTACHMENT0 + (slot - kRenderTargetSlotColor0), GL_RENDERBUFFER, (GLuint)buffer->nativePtr());
		m_attachments[slot] = buffer;
		m_attachmentIsTexture[slot] = false;
	}
	else 
	{
		return kErrorBadArgument;
	}

	return kError_SUCCESS;
}

int gpu::RenderTarget::assemble ( void )
{
	GLenum status = glCheckNamedFramebufferStatus(m_framebuffer, GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		//switch ( status )
		//{
		//case GL_FRAMEBUFFER_UNDEFINED:
		//	cout << "   bufferStatus: GL_FRAMEBUFFER_UNDEFINED" << endl; break;
		//case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		//	cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << endl; break;
		//case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		//	cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << endl; break;
		//case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		//	cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << endl; break;
		//case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		//	cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << endl; break;
		//case GL_FRAMEBUFFER_UNSUPPORTED:
		//	cout << "   bufferStatus: GL_FRAMEBUFFER_UNSUPPORTED" << endl; break;
		//case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		//	cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << endl; break;
		//case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		//	cout << "   bufferStatus: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << endl; break;
		//}
		m_assembled = false;
		return kErrorCreationFailed;
	}

	m_assembled = true;
	return kError_SUCCESS;
}

bool gpu::RenderTarget::empty ( void )
{
	// TODO: Should check the attached stuff to make sure it's not empty
	return m_framebuffer == 0 || !m_assembled;
}
bool gpu::RenderTarget::valid ( void )
{
	return m_framebuffer != 0 && m_assembled;
}

gpuHandle gpu::RenderTarget::nativePtr ( void )
{
	return m_framebuffer;
}

gpu::Texture* gpu::RenderTarget::getAttachment ( int slot )
{
	return (gpu::Texture*)m_attachments[slot];
}

gpu::WOFrameAttachment* gpu::RenderTarget::getWOAttachment ( int slot )
{
	return (gpu::WOFrameAttachment*)m_attachments[slot];
}