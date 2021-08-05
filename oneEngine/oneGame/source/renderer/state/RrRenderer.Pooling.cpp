#include "renderer/state/RrRenderer.h"

#include "core/debug/console.h"

//===============================================================================================//
// RrRenderer: Render Texture Pool
//===============================================================================================//

// Returns true if settings dropped. False otherwise.
static bool _DropSettings (rrDepthBufferRequest* io_settings)
{
	using namespace core::gfx::tex;
	if (io_settings->stencil == KStencilFormatIndex16) 
	{
		debug::Console->PrintError("Dropping Stencil16 to Stencil8.\n");
		io_settings->stencil = KStencilFormatIndex8;
		return true;
	}

	// First, attempt packed format
	if (io_settings->stencil == KStencilFormatIndex8 && (io_settings->depth == kDepthFormat32 || io_settings->depth == kDepthFormat32F))
	{
		debug::Console->PrintError("Dropping separate stencil, trying Depth32F+Stencil8.\n");
		io_settings->depth = kDepthFormat32FStencil8;
		io_settings->stencil = kStencilFormatNone;
		return true;
	}
	// Then attempt lower packed format
	if (io_settings->depth == kDepthFormat32FStencil8)
	{
		debug::Console->PrintError("Dropping Depth32F+Stencil8 to Depth24+Stencil8.\n");
		io_settings->depth = kDepthFormat24Stencil8;
		return true;
	}
	// Then go back to separate formats
	if (io_settings->depth == kDepthFormat24Stencil8)
	{
		debug::Console->PrintError("Dropping packed Depth24+Stencil8, trying Depth24 and Stencil8.\n");
		io_settings->depth = kDepthFormat24;
		io_settings->stencil = KStencilFormatIndex8;
		return true;
	}

	if (io_settings->depth == kDepthFormat24) 
	{
		debug::Console->PrintError("Dropping Depth24 to Depth16.\n");
		io_settings->depth = kDepthFormat16;
		return true;
	}

	if (io_settings->stencil == KStencilFormatIndex8) 
	{
		debug::Console->PrintError("Dropping Stencil8 to None. (This may cause visual artifacts!)\n");
		io_settings->stencil = kStencilFormatNone;
		return true;
	}

	debug::Console->PrintError("Could not downgrade screen buffer settings.\n");

	// Couldn't drop any settings.
	return false;
}

void RrRenderer::CreateRenderTexture ( rrDepthBufferRequest* in_out_request, gpu::Texture* depth, gpu::WOFrameAttachment* stencil )
{
	// Search current pool for matching request
	for (rrStoredRenderDepthTexture& rt : m_renderDepthTexturePool)
	{
		if ((frame_index - rt.frame_of_request) >= rt.persist_for
			&& rt.size == in_out_request->size
			&& rt.depth_format == in_out_request->depth
			&& rt.stencil_format == in_out_request->stencil)
		{
			*depth = rt.depth_texture;
			*stencil = rt.stencil_texture;

			rt.frame_of_request = frame_index;
			rt.persist_for = in_out_request->persist_for;
			return;
		}
	}

	// No match, need to find a properly working render target combo
	bool isValid;
	do
	{
		// allocate the textures
		if ( in_out_request->depth != core::gfx::tex::kDepthFormatNone )
			depth->allocate(core::gfx::tex::kTextureType2D, in_out_request->depth, in_out_request->size.x, in_out_request->size.y, 1, 1);
		if ( in_out_request->stencil != core::gfx::tex::kStencilFormatNone )
			stencil->allocate(core::gfx::tex::kTextureType2D, in_out_request->stencil, in_out_request->size.x, in_out_request->size.y, 1, 1);

		// Test if the textures work with a target:
		{
			gpu::RenderTarget depthTestTarget;
			depthTestTarget.create(NULL);
			if ( in_out_request->depth != core::gfx::tex::kDepthFormatNone )
				depthTestTarget.attach(gpu::kRenderTargetSlotDepth, depth);
			// Override for the combined formats
			if (in_out_request->depth == core::gfx::tex::kDepthFormat32FStencil8 || in_out_request->depth == core::gfx::tex::kDepthFormat24Stencil8)
				depthTestTarget.attach(gpu::kRenderTargetSlotStencil, depth);
			else
				depthTestTarget.attach(gpu::kRenderTargetSlotStencil, stencil);
			// "Compile" the render target.
			depthTestTarget.assemble();

			// Check if valid
			isValid = depthTestTarget.valid();
			// Free the temporary target
			depthTestTarget.destroy(NULL);
		}

		if (!isValid)
		{
			depth->free();
			stencil->free();

			// Drop the settings if still invalid
			_DropSettings(in_out_request);
		}
	}
	while (!isValid);

	// Save the allocated texture to our pool
	rrStoredRenderDepthTexture new_rt;
	new_rt.depth_format = in_out_request->depth;
	new_rt.stencil_format = in_out_request->stencil;
	new_rt.size = in_out_request->size;
	new_rt.persist_for = in_out_request->persist_for;
	new_rt.frame_of_request = frame_index;
	new_rt.depth_texture = *depth;
	new_rt.stencil_texture = *stencil;

	m_renderDepthTexturePool.push_back(new_rt);
}

void RrRenderer::CreateRenderTexture ( const rrRTBufferRequest& in_request, gpu::Texture* color )
{
	// Search current pool for matching request
	for (rrStoredRenderTexture& rt : m_renderTexturePool)
	{
		if ((frame_index - rt.frame_of_request) >= rt.persist_for
			&& rt.size == in_request.size
			&& rt.format == in_request.format)
		{
			*color = rt.texture;
			rt.frame_of_request = frame_index;
			rt.persist_for = in_request.persist_for;
			return;
		}
	}

	// No match, then make a new one
	color->allocate(core::gfx::tex::kTextureType2D, in_request.format, in_request.size.x, in_request.size.y, 1, 1);

	// Save the allocated texture to our pool
	rrStoredRenderTexture new_rt;
	new_rt.format = in_request.format;
	new_rt.frame_of_request = frame_index;
	new_rt.size = in_request.size;
	new_rt.persist_for = in_request.persist_for;
	new_rt.texture = *color;

	m_renderTexturePool.push_back(new_rt);
}

void RrRenderer::CreateRenderTextures ( const rrMRTBufferRequest& in_request, gpu::Texture* colors )
{
	for (uint i = 0; i < in_request.count; ++i)
	{
		rrRTBufferRequest request;
		request.format = in_request.formats[i];
		request.persist_for = in_request.persist_for;
		request.size = in_request.size;
		CreateRenderTexture(request, &colors[i]);
	}
}

void RrRenderer::UpdateResourcePools ( void )
{
	const int32 kTexturePersistFrameCount = 1;

	for (auto rtIterator = m_renderTexturePool.begin(); rtIterator != m_renderTexturePool.end(); )
	{
		if (rtIterator->frame_of_request + rtIterator->persist_for + kTexturePersistFrameCount <= frame_index)
		{
			rtIterator->texture.free();
			rtIterator = m_renderTexturePool.erase(rtIterator);
		}
		else
		{
			++rtIterator;
		}
	}

	for (auto rtIterator = m_renderDepthTexturePool.begin(); rtIterator != m_renderDepthTexturePool.end(); )
	{
		if (rtIterator->frame_of_request + rtIterator->persist_for + kTexturePersistFrameCount <= frame_index)
		{
			rtIterator->depth_texture.free();
			rtIterator->stencil_texture.free();
			rtIterator = m_renderDepthTexturePool.erase(rtIterator);
		}
		else
		{
			++rtIterator;
		}
	}
}