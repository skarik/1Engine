#include "renderer/gpuw/Pipeline.h"
#include "renderer/gpuw/Error.h"
#include "renderer/gpuw/Internal/Enums.h"
#include "renderer/ogl/GLCommon.h"

int gpu::Pipeline::create ( const PipelineCreationDescription* params )
{
	if (m_vao != 0)
	{
		glCreateVertexArrays(1, &m_vao);
	}

	//glVertexArrayVertexBuffer
	//glVertexArrayAttribBinding();
	//glVertexArrayVertexBuffer();

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