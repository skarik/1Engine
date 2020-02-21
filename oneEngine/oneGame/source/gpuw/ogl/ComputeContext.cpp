#include "gpuw/gpuw_common.h"
#ifdef GPU_API_OPENGL

#include "./ComputeContext.h"
#include "gpuw/Public/Error.h"

#include "./Buffers.h"

#include "./ogl/GLCommon.h"
#include "renderer/types/types.h"

#include "core/types/types.h"
#include "core/exceptions.h"

gpu::ComputeContext::ComputeContext ( void )
{
}
gpu::ComputeContext::~ComputeContext ( void )
{
}

int gpu::ComputeContext::reset ( void )
{
	//glBindVertexArray(0);
	return 0;
}

int gpu::ComputeContext::setShaderCBuffer ( ShaderStage stage, int slot, Buffer* buffer )
{
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeConstant);

	glBindBufferBase(GL_UNIFORM_BUFFER,
		(GLuint)slot,
		buffer->m_buffer);

	// todo: bind
	return kError_SUCCESS;
}

int gpu::ComputeContext::setShaderSBuffer ( ShaderStage stage, int slot, Buffer* buffer )
{
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeStructured);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
		(GLuint)slot,
		buffer->m_buffer);

	// todo: bind properly to the resource
	return kError_SUCCESS;
}

int gpu::ComputeContext::dispatch ( const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ )
{
	glDispatchCompute(groupCountX, groupCountY, groupCountZ);
	return kError_SUCCESS;
}

int gpu::ComputeContext::setIndirectArgs ( Buffer* buffer )
{
	ARCORE_ASSERT(buffer->getBufferType() != kBufferTypeUnknown);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
				 buffer->m_buffer);

	return kError_SUCCESS;
}

int gpu::ComputeContext::dispatchIndirect ( const uint32_t offset )
{
	glDispatchComputeIndirect((intptr_t)offset);
	return kError_SUCCESS;
}

int gpu::ComputeContext::submit ( void )
{
	glFlush(); // TODO: Test on all 3 drivers and possibly remove.
	return 0;
}

int gpu::ComputeContext::validate ( void )
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

#endif