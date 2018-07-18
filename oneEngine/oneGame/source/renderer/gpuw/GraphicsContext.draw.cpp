#include "GraphicsContext.h"
#include "Error.h"

#include "renderer/gpuw/Pipeline.h"
#include "renderer/gpuw/ShaderPipeline.h"

#include "renderer/ogl/GLCommon.h"

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
int gpu::GraphicsContext::setVertexBuffer ( VertexBuffer* buffer )
{
	return kError_SUCCESS;
}

int gpu::GraphicsContext::sync ( Fence* fence )
{
	return kError_SUCCESS;
}

int gpu::GraphicsContext::drawPreparePipeline ( void )
{
	if (m_pipelineBound == false)
	{
		glBindVertexArray(m_pipeline->m_vao);
	}
	return 0;
}

int gpu::GraphicsContext::draw ( const uint32_t vertexCount, const uint32_t startVertex )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		return 0;
	}
	return kErrorBadArgument;
}
int gpu::GraphicsContext::drawIndexed ( const uint32_t indexCount, const uint32_t startIndex )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		return 0;
	}
	return kErrorBadArgument;
}
int gpu::GraphicsContext::drawIndirect ( void )
{
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		return 0;
	}
	return kErrorBadArgument;
}


int gpu::GraphicsContext::submit ( void )
{
	return 0;
}