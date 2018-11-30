#include "GraphicsContext.h"
#include "Error.h"

#include "core/debug.h"

#include "renderer/gpuw/Pipeline.h"
#include "renderer/gpuw/ShaderPipeline.h"
#include "renderer/gpuw/Buffers.h"

#include "renderer/gpuw/Internal/Enums.h"

#include "renderer/ogl/GLCommon.h"

#include <stdio.h>

int gpu::GraphicsContext::reset ( void )
{
	glBindVertexArray(0);
	return 0;
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
int gpu::GraphicsContext::setVertexBuffer ( int slot, VertexBuffer* buffer, uint32_t offset )
{

	// todo: ensure pipeline exists
	glVertexArrayVertexBuffer(m_pipeline->nativePtr(),
							  slot,
							  buffer->nativePtr(),
							  offset,
							  (GLsizei)gpu::FormatGetByteStride(buffer->getFormat()));

	return kError_SUCCESS;
}

int gpu::GraphicsContext::signal ( Fence* fence )
{
	ARCORE_ERROR("not implemented");
	return kError_SUCCESS;
}
int gpu::GraphicsContext::waitOnSignal ( Fence* fence )
{
	ARCORE_ERROR("not implemented");
	return kError_SUCCESS;
}

int gpu::GraphicsContext::drawPreparePipeline ( void )
{
	if (m_pipelineBound == false)
	{
		if (m_pipeline->m_boundIndexBuffer != m_indexBuffer)
		{
			glVertexArrayElementBuffer(m_pipeline->nativePtr(), m_indexBuffer->nativePtr());
			m_pipeline->m_boundIndexBuffer = m_indexBuffer;
		}
		// TODO: loop through the vertex buffers & make sure the pipeline is setup properly.
		//		 dx11 has proper error handling for this case, but opengl does not as it forces a driver restart
		m_pipelineBound = true;
		return kError_SUCCESS;
	}
	return kError_SUCCESS;
}

//int gpu::GraphicsContext::setPrimitiveTopology ( PrimitiveTopology topology )
//{
//	m_primitiveType = topology;
//}

int gpu::GraphicsContext::draw ( const uint32_t vertexCount, const uint32_t startVertex )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		glBindVertexArray(m_pipeline->nativePtr());
		glDrawArrays(gpu::internal::ArEnumToGL(m_primitiveType),
					 (GLint)startVertex,
					 (GLsizei)vertexCount);
		ARCORE_ASSERT(validate() == 0);
		return 0;
	}
	return kErrorBadArgument;
}
int gpu::GraphicsContext::drawIndexed ( const uint32_t indexCount, const uint32_t startIndex )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		glBindVertexArray(m_pipeline->nativePtr());
		glDrawElements(gpu::internal::ArEnumToGL(m_primitiveType),
					   indexCount,
					   gpu::internal::ArEnumToGL(m_indexFormat),
					   (intptr_t)0);
		ARCORE_ASSERT(validate() == 0);
		return 0;
	}
	return kErrorBadArgument;
}
int gpu::GraphicsContext::drawIndirect ( void )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		//GL_DRAW_INDIRECT_BUFFER
		glBindVertexArray(m_pipeline->nativePtr());
		glDrawElementsIndirect(gpu::internal::ArEnumToGL(m_primitiveType),
							   gpu::internal::ArEnumToGL(m_indexFormat),
							   (intptr_t)0);
		ARCORE_ASSERT(validate() == 0);
		return 0;
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
	glFlush();
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