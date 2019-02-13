#include "renderer/gpuw/Pipeline.h"
#include "renderer/gpuw/Error.h"
#include "renderer/gpuw/Internal/Enums.h"
#include "renderer/gpuw/ogl/GLCommon.h"

int gpu::Pipeline::create ( Device* device, const PipelineCreationDescription* params )
{
	if (m_vao == 0)
	{
		glCreateVertexArrays(1, &m_vao);
	}

	m_pipeline = params->shader_pipeline;

	ia_topology = params->ia_topology;
	ia_primitiveRestartEnable = params->ia_primitiveRestartEnable;

	for (uint32_t i = 0; i < params->vv_inputAttributesCount; ++i)
	{
		GLint size = gpu::FormatComponentCount(params->vv_inputAttributes[i].format);
		GLenum type = gpu::internal::ArFormatToGlDataType(params->vv_inputAttributes[i].format);
		GLboolean normalized = GL_FALSE;

		glEnableVertexArrayAttrib(m_vao,
								  params->vv_inputAttributes[i].location);

		glVertexArrayAttribFormat(m_vao,
								  params->vv_inputAttributes[i].location,
								  size,
								  type,
								  normalized, 
								  params->vv_inputAttributes[i].offset);

		glVertexArrayAttribBinding(m_vao,
								   params->vv_inputAttributes[i].location,
								   params->vv_inputAttributes[i].binding);
	}

	return kError_SUCCESS;
}

int gpu::Pipeline::destroy ( Device* device )
{
	glDeleteVertexArrays(1, &m_vao);
	m_vao = 0;
	return kError_SUCCESS;
}

//	nativePtr() : returns native index or pointer to the resource.
gpuHandle gpu::Pipeline::nativePtr ( void )
{
	return m_vao;
}

//	valid() : is this pipeline valid to be used?
// If the pipeline failed to be created or doesnt exist, this will be false
bool gpu::Pipeline::valid ( void )
{
	return m_vao != 0 && m_pipeline != NULL;
}