#ifndef RENDERER_TYPES_RR_RENDER_CONTEXT_H_
#define RENDERER_TYPES_RR_RENDER_CONTEXT_H_

namespace gpu {
	class GraphicsContext;
}
class rrSingleFrameConstantBufferPool;

// Render context that's passed into params
struct rrRenderContext
{
	gpu::GraphicsContext*
					context_graphics = nullptr;
	rrSingleFrameConstantBufferPool*
					constantBuffer_pool = nullptr;
};

#endif//RENDERER_TYPES_RR_RENDER_CONTEXT_H_