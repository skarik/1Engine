#ifndef RENDERER_STATE_RAII_HELPERS_H_
#define RENDERER_STATE_RAII_HELPERS_H_

#include "renderer/types/types.h"
#include "gpuw/Buffers.h"
#include "gpuw/Texture.h"
#include "gpuw/RenderTarget.h"

class rrRenderTarget
{
public:
	explicit RENDER_API		rrRenderTarget ( gpu::Texture* depthBuffer, gpu::WOFrameAttachment* stencilBuffer )
	{
		m_renderTarget.create(NULL);
		m_renderTarget.attach(gpu::kRenderTargetSlotDepth, depthBuffer);
		if (stencilBuffer && stencilBuffer->valid())
		{
			m_renderTarget.attach(gpu::kRenderTargetSlotStencil, stencilBuffer);
		}
		m_renderTarget.assemble();
		ARCORE_ASSERT(m_renderTarget.valid());
	}

	explicit RENDER_API		rrRenderTarget ( gpu::Texture* depthBuffer, gpu::WOFrameAttachment* stencilBuffer, gpu::Texture* colorBuffer )
	{
		m_renderTarget.create(NULL);
		m_renderTarget.attach(gpu::kRenderTargetSlotDepth, depthBuffer);
		if (stencilBuffer && stencilBuffer->valid())
		{
			m_renderTarget.attach(gpu::kRenderTargetSlotStencil, stencilBuffer);
		}
		m_renderTarget.attach(gpu::kRenderTargetSlotColor0, colorBuffer);
		m_renderTarget.assemble();
		ARCORE_ASSERT(m_renderTarget.valid());
	}

	explicit RENDER_API		rrRenderTarget ( gpu::Texture* depthBuffer, gpu::WOFrameAttachment* stencilBuffer, gpu::Texture* colorBuffers, const uint colorBufferCount )
	{
		m_renderTarget.create(NULL);
		m_renderTarget.attach(gpu::kRenderTargetSlotDepth, depthBuffer);
		if (stencilBuffer && stencilBuffer->valid())
		{
			m_renderTarget.attach(gpu::kRenderTargetSlotStencil, stencilBuffer);
		}
		for (uint i = 0; i < colorBufferCount; ++i)
		{
			m_renderTarget.attach(gpu::kRenderTargetSlotColor0 + i, &colorBuffers[i]);
		}
		m_renderTarget.assemble();
		ARCORE_ASSERT(m_renderTarget.valid());
	}

	explicit RENDER_API		rrRenderTarget ( gpu::Texture* colorBuffer )
	{
		m_renderTarget.create(NULL);
		m_renderTarget.attach(gpu::kRenderTargetSlotColor0, colorBuffer);
		m_renderTarget.assemble();
		ARCORE_ASSERT(m_renderTarget.valid());
	}

	RENDER_API				~rrRenderTarget ( void )
	{
		m_renderTarget.destroy(NULL);
	}

public:
	gpu::RenderTarget	m_renderTarget;
};

#endif//RENDERER_STATE_RAII_HELPERS_H_