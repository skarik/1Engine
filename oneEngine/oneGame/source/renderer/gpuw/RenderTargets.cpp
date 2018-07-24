#include "renderer/gpuw/RenderTargets.h"
#include "renderer/gpuw/Error.h"
#include "renderer/ogl/GLCommon.h"

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
		glNamedFramebufferTexture(m_framebuffer, GL_DEPTH_ATTACHMENT, texture->nativePtr(), 0);
	}
	else if (slot == kRenderTargetSlotStencil)
	{
		glNamedFramebufferTexture(m_framebuffer, GL_STENCIL_ATTACHMENT, texture->nativePtr(), 0);
	}
	else if (slot >= kRenderTargetSlotColor0)
	{
		glNamedFramebufferTexture(m_framebuffer, GL_COLOR_ATTACHMENT0 + (slot - kRenderTargetSlotColor0), texture->nativePtr(), 0);
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
		glNamedFramebufferRenderbuffer(m_framebuffer, GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, buffer->nativePtr());
	}
	else if (slot == kRenderTargetSlotStencil)
	{
		glNamedFramebufferTexture(m_framebuffer, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer->nativePtr());
	}
	else if (slot >= kRenderTargetSlotColor0)
	{
		glNamedFramebufferTexture(m_framebuffer, GL_COLOR_ATTACHMENT0 + (slot - kRenderTargetSlotColor0), GL_RENDERBUFFER, buffer->nativePtr());
	}
	else 
	{
		return kErrorBadArgument;
	}
	return 0;
}

bool gpu::RenderTarget::assemble ( void )
{
	GLenum status = glCheckNamedFramebufferStatus( m_framebuffer );
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
		return false;
	}
	return true;
}