#ifndef RENDERER_BUFFER_CHAIN_FOR_HYBRID_PIPELINE_
#define RENDERER_BUFFER_CHAIN_FOR_HYBRID_PIPELINE_

#include "core/math/Math3d.h"
#include "core/math/vect2d_template.h"
#include "renderer/gpuw/Textures.h"
#include "renderer/gpuw/RenderTargets.h"
#include "renderer/state/InternalSettings.h"

class RrHybridBufferChain
{
public:
	// Creates buffers for rendering to.
	//RENDER_API void			CreateTargetBuffers ( void );
	// Recreates buffers for the given chain. Returns success.
	RENDER_API bool			CreateTargetBufferChain ( renderer::internalSettings_t settings, const Vector2i& size );
	bool					CreateTargetBufferChain_Internal ( const renderer::internalSettings_t& settings, const Vector2i& size );
	/*RENDER_API gpu::RenderTarget*
							GetForwardBuffer ( void );
	RENDER_API gpu::RenderTarget*
							GetDeferredBuffer ( void );
	RENDER_API gpu::Texture*
							GetDepthTexture ( void );
	RENDER_API gpu::WOFrameAttachment*
							GetStencilTexture ( void );*/

public:
	enum rrConstants : int
	{
		kMRTColorAttachmentCount = 4,
	};

	//glHandle		buffer_depth;
	//glHandle		buffer_stencil;
	gpu::Texture		buffer_depth;
	gpu::WOFrameAttachment
						buffer_stencil;
	//RrRenderTexture*	buffer_forward_rt;
	gpu::Texture		buffer_color;
	gpu::RenderTarget	buffer_forward_rt;

	//CMRTTexture*	buffer_deferred_mrt;
	//RrRenderTexture*	buffer_deferred_rt;
	gpu::Texture		buffer_deferred_color_composite;
	gpu::Texture		buffer_deferred_color[kMRTColorAttachmentCount];
	gpu::RenderTarget	buffer_deferred_mrt;
	gpu::RenderTarget	buffer_deferred_rt;
};

#endif//RENDERER_BUFFER_CHAIN_FOR_HYBRID_PIPELINE_