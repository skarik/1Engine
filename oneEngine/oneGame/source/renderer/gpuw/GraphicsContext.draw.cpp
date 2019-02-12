#include "GraphicsContext.h"
#include "Error.h"

#include "core/debug.h"

#include "renderer/gpuw/RenderTarget.h"
#include "renderer/gpuw/Pipeline.h"
#include "renderer/gpuw/ShaderPipeline.h"
#include "renderer/gpuw/Sampler.h"
#include "renderer/gpuw/Buffers.h"
#include "renderer/gpuw/Fence.h"

#include "renderer/gpuw/Internal/Enums.h"

#include "renderer/ogl/GLCommon.h"

#include <stdio.h>

int gpu::GraphicsContext::reset ( void )
{
	glBindVertexArray(0);
	return 0;
}

int gpu::GraphicsContext::setRenderTarget ( RenderTarget* renderTarget )
{
	ARCORE_ASSERT(renderTarget != NULL);

	// We through the attachments on the render target to generate the list of actual targets.
	// TODO: This could be pregenerated on the RenderTarget themselves on their assembly. This would also allow removing the awful 0xFFFFFFFF hack.
	GLsizei attachmentCount = 0;
	GLuint attachments [16] = {};
	
	for (GLsizei i = 0; i < 16; ++i)
	{
		if (renderTarget->m_attachments[i] != NULL) {
			attachments[attachmentCount] = GL_COLOR_ATTACHMENT0 + i;
			++attachmentCount;
		}
	}

	if (renderTarget->m_framebuffer != 0xFFFFFFFF)
	{
		// Set the framebuffer as the given target.
		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->m_framebuffer);
		glNamedFramebufferDrawBuffers(renderTarget->m_framebuffer, attachmentCount, attachments);
		//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	}
	else
	{
		// Set the framebuffer to the screen's target.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glNamedFramebufferDrawBuffer(0, GL_FRONT);
		glDrawBuffer(GL_FRONT_LEFT);
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setPipeline ( Pipeline* pipeline )
{
	if (pipeline != m_pipeline)
	{
		m_pipeline = pipeline;
		m_pipelineBound = false;
		m_pipelineDataBound = false;
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setIndexBuffer ( Buffer* buffer, IndexFormat format )
{
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeIndex);

	// TODO:

	m_indexBuffer = buffer;
	m_indexFormat = format;

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setVertexBuffer ( int slot, Buffer* buffer, uint32_t offset )
{
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeVertex);

	// todo: ensure pipeline exists
	glVertexArrayVertexBuffer((GLuint)m_pipeline->nativePtr(),
							  (GLuint)slot,
							  (GLuint)buffer->nativePtr(),
							  (GLintptr)offset,
							  (GLsizei)gpu::FormatGetByteStride(buffer->getFormat()));

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderCBuffer ( ShaderStage stage, int slot, Buffer* buffer )
{
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeConstant);

	glBindBufferBase(GL_UNIFORM_BUFFER,
					 (GLuint)slot,
					 buffer->m_buffer);

	// todo: bind
	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderSBuffer ( ShaderStage stage, int slot, Buffer* buffer )
{
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeStructured);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
					 (GLuint)slot,
					 buffer->m_buffer);

	// todo: bind properly to the resource
	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderSamplerAuto ( ShaderStage stage, int slot, Texture* buffer )
{
	ARCORE_ASSERT(buffer != NULL);

	//todo: proper sampler handling
	if (buffer != NULL)
		glBindTextureUnit(slot, (GLuint)buffer->nativePtr());
	else
		glBindTextureUnit(slot, 0);

	return kError_SUCCESS;
}

int gpu::GraphicsContext::drawPreparePipeline ( void )
{
	if (m_pipelineBound == false)
	{
		glUseProgram((GLuint)m_pipeline->m_pipeline->m_program);

		if (m_pipeline->m_boundIndexBuffer != m_indexBuffer)
		{
			glVertexArrayElementBuffer((GLuint)m_pipeline->nativePtr(), (GLuint)m_indexBuffer->nativePtr());
			m_pipeline->m_boundIndexBuffer = m_indexBuffer;
		}
		
		// Set up the prim restart (TODO: make this a context state to avoid extra GL calls)
		if (m_pipeline->ia_primitiveRestartEnable)
		{
			glEnable(GL_PRIMITIVE_RESTART);
			glPrimitiveRestartIndex((m_indexBuffer->getFormat() == kFormatR16UInteger) ? 0xFFFF : 0xFFFFFFFF);
		}
		else
		{
			glDisable(GL_PRIMITIVE_RESTART);
		}

		// Update topology
		m_primitiveType = m_pipeline->ia_topology;

		// TODO: loop through the vertex buffers & make sure the pipeline is setup properly.
		//		 dx11 has proper error handling for this case, but opengl does not as it forces a driver restart
		m_pipelineBound = true;
		return kError_SUCCESS;
	}
	return kError_SUCCESS;
}

int gpu::GraphicsContext::draw ( const uint32_t vertexCount, const uint32_t startVertex )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		glBindVertexArray((GLuint)m_pipeline->nativePtr());
		glDrawArrays(gpu::internal::ArEnumToGL(m_primitiveType),
					 (GLint)startVertex,
					 (GLsizei)vertexCount);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}
int gpu::GraphicsContext::drawIndexed ( const uint32_t indexCount, const uint32_t startIndex )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		glBindVertexArray((GLuint)m_pipeline->nativePtr());
		glDrawElements(gpu::internal::ArEnumToGL(m_primitiveType),
					   indexCount,
					   gpu::internal::ArEnumToGL(m_indexFormat),
					   (intptr_t)0);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}

int gpu::GraphicsContext::drawIndexedInstanced ( const uint32_t indexCount, const uint32_t instanceCount, const uint32_t startIndex )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		glBindVertexArray((GLuint)m_pipeline->nativePtr());
		glDrawElementsInstanced(gpu::internal::ArEnumToGL(m_primitiveType),
								indexCount,
								gpu::internal::ArEnumToGL(m_indexFormat),
								(intptr_t)0,
								instanceCount);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}

int gpu::GraphicsContext::drawIndirect ( void )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		ARCORE_ERROR("Not implemented");
		//GL_DRAW_INDIRECT_BUFFER
		glBindVertexArray((GLuint)m_pipeline->nativePtr());
		glDrawElementsIndirect(gpu::internal::ArEnumToGL(m_primitiveType),
							   gpu::internal::ArEnumToGL(m_indexFormat),
							   (intptr_t)0);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}

int gpu::GraphicsContext::clearDepthStencil ( bool clearDepth, float depth, bool clearStencil, uint8_t stencil )
{
	GLbitfield clearMask = 0;
	if (clearDepth)
	{
		clearMask |= GL_DEPTH_BUFFER_BIT;
		glDepthMask(GL_TRUE);
		glClearDepth(depth);
	}
	if (clearStencil)
	{
		clearMask |= GL_STENCIL_BUFFER_BIT;
		glStencilMask(GL_TRUE);
		glClearStencil(stencil);
	}
	if (clearMask != 0)
	{
		glClear(clearMask);
	}
	return 0;
}
int gpu::GraphicsContext::clearColor ( float* rgbaColor )
{
	glClearColor(rgbaColor[0], rgbaColor[1], rgbaColor[2], rgbaColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	return 0;
}

int gpu::GraphicsContext::submit ( void )
{
	glFlush(); // TODO: Test on all 3 drivers and possibly remove.
	return 0;
}

int gpu::GraphicsContext::validate ( void )
{
	GLenum error;
	bool hasError = false;

	// Pop all the errors off the stack to check
	do
	{
		error = glGetError();
		if ( error != GL_NO_ERROR ) {
			hasError = true;
		}

		switch (error)
		{
		case GL_NO_ERROR: break;
		case GL_INVALID_ENUM:
			fprintf( stderr, "gl: GL_INVALID_ENUM\n" );
			//"An unacceptable value is specified for an enumerated argument. The offending function is ignored, having no side effect other than to set the error flag." 
			break;
		case GL_INVALID_VALUE:
			fprintf( stderr, "gl: GL_INVALID_VALUE\n" );
			//"A numeric argument is out of range. The offending function is ignored, having no side effect other than to set the error flag."
			break;
		case GL_INVALID_OPERATION:
			fprintf( stderr, "gl: GL_INVALID_OPERATION\n" );
			//"The specified operation is not allowed in the current state. The offending function is ignored, having no side effect other than to set the error flag."
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			fprintf( stderr, "gl: GL_INVALID_FRAMEBUFFER_OPERATION\n" );
			//"The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag."
			break;
		case GL_OUT_OF_MEMORY:
			fprintf( stderr, "gl: GL_OUT_OF_MEMORY\n" );
			//"There is not enough memory left to execute the function. The state of OpenGL is undefined, except for the state of the error flags, after this error is recorded."
			break;
		case GL_STACK_UNDERFLOW:
			fprintf( stderr, "gl: GL_STACK_UNDERFLOW\n" );
			//"This function would cause a stack underflow. The offending function is ignored, having no side effect other than to set the error flag."
			break;
		case GL_STACK_OVERFLOW:
			fprintf( stderr, "gl: GL_STACK_OVERFLOW\n" );
			//"This function would cause a stack overflow. The offending function is ignored, having no side effect other than to set the error flag."
			break;
		//case GL_TABLE_TOO_LARGE:
		//	fprintf( stderr, "gl: GL_TABLE_TOO_LARGE\n" );
		//	//"The specified table exceeds the implementation's maximum supported table size. The offending command is ignored and has no other side effect than to set the error flag."
		//	break;
		default:
			fprintf( stderr, "gl: UNKNOWN ERROR\n" );
			break;
		}
	}
	while ( error != GL_NO_ERROR );

	// If there's an error, throw exception
	if ( hasError )
	{
		return kErrorDeviceError;
	}
	return 0;
}