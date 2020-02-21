#include "gpuw/gpuw_common.h"
#ifdef GPU_API_OPENGL

#include "core/types/types.h"
#include "core/exceptions.h"

#include "gpuw/Public/Error.h"

#include "./ComputeContext.h"
#include "./Buffers.h"
#include "./Pipeline.h"
#include "./ShaderPipeline.h"
#include "./Texture.h"
#include "./Sampler.h"
#include "./ogl/GLCommon.h"

#include "renderer/types/types.h"

gpu::ComputeContext::ComputeContext ( void )
	: m_pipeline(NULL), m_pipelineBound(false), m_pipelineDataBound(false)
{
	SamplerCreationDescription scd = SamplerCreationDescription();
	m_defaultSampler = new Sampler;
	m_defaultSampler->create(NULL, &scd);
}
gpu::ComputeContext::~ComputeContext ( void )
{
	m_defaultSampler->destroy(NULL);
	delete m_defaultSampler;
}

int gpu::ComputeContext::reset ( void )
{
	//glBindVertexArray(0);
	return 0;
}

int gpu::ComputeContext::setPipeline ( Pipeline* pipeline )
{
	ARCORE_ASSERT(pipeline != NULL);
	ARCORE_ASSERT(pipeline->m_pipeline->m_type == kPipelineTypeCompute);
	if (pipeline != m_pipeline)
	{
		m_pipeline = pipeline;
		m_pipelineBound = false;
		m_pipelineDataBound = false;
	}
	return kError_SUCCESS;
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

int gpu::ComputeContext::setShaderSampler ( ShaderStage stage, int slot, Sampler* sampler )
{
	ARCORE_ASSERT(sampler != NULL);
	glBindSampler(slot, (GLuint)sampler->nativePtr());
	return kError_SUCCESS;
}

int gpu::ComputeContext::setShaderTexture ( ShaderStage stage, int slot, Texture* texture )
{
	ARCORE_ASSERT(texture != NULL);
	glBindTextureUnit(slot, (GLuint)texture->nativePtr());
	return kError_SUCCESS;
}

int gpu::ComputeContext::setShaderTextureAuto ( ShaderStage stage, int slot, Texture* texture )
{
	ARCORE_ASSERT(texture != NULL);

	if (texture != NULL) {
		glBindSampler(slot, (GLuint)m_defaultSampler->nativePtr());
		glBindTextureUnit(slot, (GLuint)texture->nativePtr());
	}
	else {
		glBindSampler(slot, 0);
		glBindTextureUnit(slot, 0);
	}

	return kError_SUCCESS;
}

int gpu::ComputeContext::setShaderResource ( ShaderStage stage, int slot, Buffer* buffer )
{
	ARCORE_ASSERT(buffer->getBufferType() != kBufferTypeUnknown);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
		(GLuint)slot,
		buffer->m_buffer);

	// todo: bind properly to the resource
	return kError_SUCCESS;
}

int gpu::ComputeContext::dispatchPreparePipeline ( void )
{
	if (m_pipelineBound == false)
	{
		glUseProgram((GLuint)m_pipeline->m_pipeline->m_program);
		m_pipelineBound = true;
		return kError_SUCCESS;
	}
	return kError_SUCCESS;
}

int gpu::ComputeContext::dispatch ( const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ )
{
	if (dispatchPreparePipeline() == kError_SUCCESS)
	{
		glDispatchCompute(groupCountX, groupCountY, groupCountZ);
	}
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
	if (dispatchPreparePipeline() == kError_SUCCESS)
	{
		glDispatchComputeIndirect((intptr_t)offset);
	}
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