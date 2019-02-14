#ifndef RENDERER_BUFFER_CHAIN_FOR_HYBRID_PIPELINE_
#define RENDERER_BUFFER_CHAIN_FOR_HYBRID_PIPELINE_

#include "core/math/Math3d.h"
#include "core/math/vect2d_template.h"
#include "gpuw/Public/Error.h"
#include "gpuw/Texture.h"
#include "gpuw/RenderTarget.h"
#include "renderer/state/InternalSettings.h"

class RrHybridBufferChain
{
public:
	// Creates buffers for rendering to.
	//RENDER_API void			CreateTargetBuffers ( void );
	//

	//	CreateTargetBufferChain(io_settings, size) : Recreates buffers for the given chain.
	// Will create new buffers using the options given in io_settings, dropping options until creation works.
	// Arguments:
	//	io_settings:	Input settings. Will be changed by the call to reflect what was actually created.
	//	size:			Size in pixels of buffer to be created.
	// Returns:
	//	ErrorCode: Describes success or failure of the call.
	RENDER_API gpu::ErrorCode
							CreateTargetBufferChain ( renderer::rrInternalSettings* io_settings, const Vector2i& size );
	bool					CreateTargetBufferChain_Internal ( const renderer::rrInternalSettings* settings, const Vector2i& size );

	/*RENDER_API gpu::RenderTarget*
							GetForwardBuffer ( void );
	RENDER_API gpu::RenderTarget*
							GetDeferredBuffer ( void );
	RENDER_API gpu::Texture*
							GetDepthTexture ( void );
	RENDER_API gpu::WOFrameAttachment*
							GetStencilTexture ( void );*/

	bool					FreeTargetBufferChain ( void );

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