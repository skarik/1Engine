#include "renderer/state/RrHybridBufferChain.h"

#include "core/debug/console.h"

// Internal buffer handling
// ================================

// Returns true if settings dropped. False otherwise.
static bool _DropSettings (renderer::rrInternalSettings* io_settings)
{
	using namespace core::gfx::tex;
	if (io_settings->mainStencilFormat == KStencilFormatIndex16) 
	{
		debug::Console->PrintError("Dropping Stencil16 to Stencil8.");
		io_settings->mainStencilFormat = KStencilFormatIndex8;
		return true;
	}

	if (io_settings->mainDepthFormat == kDepthFormat32) 
	{
		debug::Console->PrintError("Dropping Depth32 to Depth16.");
		io_settings->mainDepthFormat = kDepthFormat16;
		return true;
	}

	if (io_settings->mainStencilFormat == KStencilFormatIndex8) 
	{
		debug::Console->PrintError("Dropping Stencil8 to None. (This may cause visual artifacts!)");
		io_settings->mainStencilFormat = kStencilFormatNone;
		return true;
	}

	debug::Console->PrintError("Could not downgrade screen buffer settings.");

	// Couldn't drop any settings.
	return false;
}
/*
void RrHybridBufferChain::CreateTargetBuffers ( void )
{
	if (internal_chain_list.empty())
	{
		rrInternalBufferChain chain = {};
		// Three backbuffers
		internal_chain_list.push_back(chain);
		internal_chain_list.push_back(chain);
		internal_chain_list.push_back(chain);
		// Set current buffer to first
		internal_chain_current = &internal_chain_list[0];
	}
	for (rrInternalBufferChain& chain : internal_chain_list)
	{
		bool status = CreateTargetBufferChain(chain);
		if (status == false)
		{
			// There was an error in creating the target buffer chain. We need to break, try another set of formats, then continue.
			if (_DropSettings(internal_settings))
			{
				debug::Console->PrintError("Screen buffer formats not supported. Dropping settings and attempting again.");
				// Attempt to create again.
				CreateTargetBuffers();
				// Stop the loop.
				break; 
			}
			else
			{
				debug::Console->PrintError("Screen buffer formats not supported. Throwing an unsupported error.");
				throw core::DeprecatedFeatureException();
			}
		}
	}
}*/
gpu::ErrorCode RrHybridBufferChain::CreateTargetBufferChain ( renderer::rrInternalSettings* io_settings, const Vector2i& size )
{
	bool status;
	do
	{
		status = CreateTargetBufferChain_Internal(io_settings, size);
		if (status == false)
		{
			// There was an error in creating the target buffer chain. We need to break, try another set of formats, then continue.
			if (_DropSettings(io_settings))
			{
				debug::Console->PrintError("Screen buffer formats not supported. Dropping settings and attempting again.");
				// Attempt to create again.
				continue;
			}
			else
			{
				debug::Console->PrintError("Screen buffer formats not supported. Throwing an unsupported error.");
				throw core::DeprecatedFeatureException();
				return gpu::kErrorFormatUnsupported;
			}
		}
	}
	while (status == false);

	return gpu::kError_SUCCESS;
}
bool RrHybridBufferChain::CreateTargetBufferChain_Internal ( const renderer::rrInternalSettings* settings, const Vector2i& size )
{
	// Delete shared buffers
	{
		buffer_color.free();
		buffer_deferred_color_composite.free();
		buffer_depth.free();
		buffer_stencil.free();

		for (int i = 0; i < kMRTColorAttachmentCount; ++i)
			buffer_deferred_color[i].free();
	}
	// Delete forward buffers
	if ( buffer_forward_rt.empty() == false )
	{
		buffer_forward_rt.destroy(NULL);
	}
	// Delete deferred buffers
	if ( buffer_deferred_mrt.empty() == false )
	{
		buffer_deferred_mrt.destroy(NULL);
		buffer_deferred_rt.destroy(NULL);
	}

	// Create forward buffers
	if ( buffer_forward_rt.empty() )
	{
		// Generate unique color buffer
		buffer_color.allocate(core::gfx::tex::kTextureType2D, settings->mainColorAttachmentFormat, size.x, size.y);

		// Generate shared depth and stencil buffers (these are also used by the MRT)
		if ( settings->mainDepthFormat != core::gfx::tex::kDepthFormatNone )
			buffer_depth.allocate(core::gfx::tex::kTextureType2D, settings->mainDepthFormat, size.x, size.y);
		if ( settings->mainStencilFormat != core::gfx::tex::kStencilFormatNone )
			buffer_stencil.allocate(core::gfx::tex::kTextureType2D, settings->mainStencilFormat, size.x, size.y);

		buffer_forward_rt.create(NULL);
		// Put the buffer together
		buffer_forward_rt.attach(gpu::kRenderTargetSlotColor0, &buffer_color);
		buffer_forward_rt.attach(gpu::kRenderTargetSlotDepth, &buffer_depth);
		buffer_forward_rt.attach(gpu::kRenderTargetSlotStencil, &buffer_stencil);
		// "Compile" the render target.
		buffer_forward_rt.assemble();

		// Check to make sure buffer is valid.
		if (!buffer_forward_rt.valid())
		{
			return false;
		}
	}
	// Create deferred buffers
	if ( buffer_deferred_mrt.empty() )
	{
		// Generate unique color buffer
		buffer_deferred_color_composite.allocate(core::gfx::tex::kTextureType2D, settings->mainColorAttachmentFormat, size.x, size.y);

		buffer_deferred_rt.create(NULL);
		// Put the buffer together
		buffer_deferred_rt.attach(gpu::kRenderTargetSlotColor0, &buffer_deferred_color_composite);
		buffer_deferred_rt.attach(gpu::kRenderTargetSlotDepth, &buffer_depth);
		buffer_deferred_rt.attach(gpu::kRenderTargetSlotStencil, &buffer_stencil);
		// "Compile" the render target.
		buffer_deferred_rt.assemble();

		// TODO: Make configurable
		buffer_deferred_color[0].allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA8, size.x, size.y);
		buffer_deferred_color[1].allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA16F, size.x, size.y);
		buffer_deferred_color[2].allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA8, size.x, size.y);
		buffer_deferred_color[3].allocate(core::gfx::tex::kTextureType2D, core::gfx::tex::kColorFormatRGBA8, size.x, size.y);

		buffer_deferred_mrt.create(NULL);
		// Attach the color buffers for the MRT
		for (int i = 0; i < kMRTColorAttachmentCount; ++i)
			buffer_deferred_mrt.attach(gpu::kRenderTargetSlotColor0 + i, &buffer_deferred_color[i]);
		// Add the shared depth & stencil
		buffer_deferred_mrt.attach(gpu::kRenderTargetSlotDepth, &buffer_depth);
		buffer_deferred_mrt.attach(gpu::kRenderTargetSlotStencil, &buffer_stencil);
		// "Compile" the render target.
		buffer_deferred_mrt.assemble();

		// Check to make sure buffer is valid.
		if (!buffer_deferred_mrt.valid())
		{
			return false;
		}
	}
	// Made it here, so return success.
	return true;
}

bool RrHybridBufferChain::FreeTargetBufferChain ( void )
{
	// Delete shared buffers
	{
		buffer_color.free();
		buffer_deferred_color_composite.free();
		buffer_depth.free();
		buffer_stencil.free();

		for (int i = 0; i < kMRTColorAttachmentCount; ++i)
			buffer_deferred_color[i].free();
	}
	// Delete forward buffers
	if ( buffer_forward_rt.empty() == false )
	{
		buffer_forward_rt.destroy(NULL);
	}
	// Delete deferred buffers
	if ( buffer_deferred_mrt.empty() == false )
	{
		buffer_deferred_mrt.destroy(NULL);
		buffer_deferred_rt.destroy(NULL);
	}
	// Made it here, so return success.
	return true;
}