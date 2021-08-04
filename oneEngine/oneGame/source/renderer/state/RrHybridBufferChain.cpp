#include "renderer/state/RrHybridBufferChain.h"

#include "core/debug/console.h"

const core::gfx::tex::arColorFormat rrBufferChainInfo::kDefaultColorAttachments[4] = {
	core::gfx::tex::kColorFormatRGBA8,
	core::gfx::tex::kColorFormatRGBA16F,
	core::gfx::tex::kColorFormatRGBA8,
	core::gfx::tex::kColorFormatRGBA8};

// Internal buffer handling
// ================================

// Returns true if settings dropped. False otherwise.
static bool _DropSettings (rrBufferChainInfo* io_settings)
{
	using namespace core::gfx::tex;
	if (io_settings->stencilFormat == KStencilFormatIndex16) 
	{
		debug::Console->PrintError("Dropping Stencil16 to Stencil8.\n");
		io_settings->stencilFormat = KStencilFormatIndex8;
		return true;
	}

	// First, attempt packed format
	if (io_settings->stencilFormat == KStencilFormatIndex8 && (io_settings->depthFormat == kDepthFormat32 || io_settings->depthFormat == kDepthFormat32F))
	{
		debug::Console->PrintError("Dropping separate stencil, trying Depth32F+Stencil8.\n");
		io_settings->depthFormat = kDepthFormat32FStencil8;
		io_settings->stencilFormat = kStencilFormatNone;
		return true;
	}
	// Then attempt lower packed format
	if (io_settings->depthFormat == kDepthFormat32FStencil8)
	{
		debug::Console->PrintError("Dropping Depth32F+Stencil8 to Depth24+Stencil8.\n");
		io_settings->depthFormat = kDepthFormat24Stencil8;
		return true;
	}
	// Then go back to separate formats
	if (io_settings->depthFormat == kDepthFormat24Stencil8)
	{
		debug::Console->PrintError("Dropping packed Depth24+Stencil8, trying Depth24 and Stencil8.\n");
		io_settings->depthFormat = kDepthFormat24;
		io_settings->stencilFormat = KStencilFormatIndex8;
		return true;
	}

	if (io_settings->depthFormat == kDepthFormat24) 
	{
		debug::Console->PrintError("Dropping Depth24 to Depth16.\n");
		io_settings->depthFormat = kDepthFormat16;
		return true;
	}

	if (io_settings->stencilFormat == KStencilFormatIndex8) 
	{
		debug::Console->PrintError("Dropping Stencil8 to None. (This may cause visual artifacts!)\n");
		io_settings->stencilFormat = kStencilFormatNone;
		return true;
	}

	debug::Console->PrintError("Could not downgrade screen buffer settings.\n");

	// Couldn't drop any settings.
	return false;
}

gpu::ErrorCode RrHybridBufferChain::CreateTargetBufferChain ( rrBufferChainInfo* io_settings, const Vector2i& size )
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
				debug::Console->PrintError("Screen buffer formats not supported. Dropping settings and attempting again.\n");
				// Attempt to create again.
				continue;
			}
			else
			{
				debug::Console->PrintError("Screen buffer formats not supported. Throwing an unsupported error.\n");
				throw core::DeprecatedFeatureException();
				return gpu::kErrorFormatUnsupported;
			}
		}
	}
	while (status == false);

	return gpu::kError_SUCCESS;
}
bool RrHybridBufferChain::CreateTargetBufferChain_Internal ( const rrBufferChainInfo* settings, const Vector2i& size )
{
	// Delete shared buffers
	{
		texture_color.free();
		texture_deferred_color_composite.free();
		texture_depth.free();
		texture_stencil.free();

		for (int i = 0; i < kMRTColorAttachmentCount; ++i)
			texture_deferred_color[i].free();
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
		texture_color.allocate(core::gfx::tex::kTextureType2D, settings->mainAttachmentFormat, size.x, size.y, 1, 1);

		// Generate shared depth and stencil buffers (these are also used by the MRT)
		if ( settings->depthFormat != core::gfx::tex::kDepthFormatNone )
			texture_depth.allocate(core::gfx::tex::kTextureType2D, settings->depthFormat, size.x, size.y, 1, 1);
		if ( settings->stencilFormat != core::gfx::tex::kStencilFormatNone )
			texture_stencil.allocate(core::gfx::tex::kTextureType2D, settings->stencilFormat, size.x, size.y, 1, 1);

		buffer_forward_rt.create(NULL);
		// Put the buffer together
		buffer_forward_rt.attach(gpu::kRenderTargetSlotColor0, &texture_color);
		buffer_forward_rt.attach(gpu::kRenderTargetSlotDepth, &texture_depth);
		// Override for the combined formats (TODO: Check if this is valid)
		if (settings->depthFormat == core::gfx::tex::kDepthFormat32FStencil8 || settings->depthFormat == core::gfx::tex::kDepthFormat24Stencil8)
			buffer_forward_rt.attach(gpu::kRenderTargetSlotStencil, &texture_depth);
		else
			buffer_forward_rt.attach(gpu::kRenderTargetSlotStencil, &texture_stencil);
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
		texture_deferred_color_composite.allocate(core::gfx::tex::kTextureType2D, settings->mainAttachmentFormat, size.x, size.y, 1, 1);

		buffer_deferred_rt.create(NULL);
		// Put the buffer together
		buffer_deferred_rt.attach(gpu::kRenderTargetSlotColor0, &texture_deferred_color_composite);
		buffer_deferred_rt.attach(gpu::kRenderTargetSlotDepth, &texture_depth);
		// Override for the combined formats (TODO: Check if this is valid)
		if (settings->depthFormat == core::gfx::tex::kDepthFormat32FStencil8 || settings->depthFormat == core::gfx::tex::kDepthFormat24Stencil8)
			buffer_deferred_rt.attach(gpu::kRenderTargetSlotStencil, &texture_depth);
		else
			buffer_deferred_rt.attach(gpu::kRenderTargetSlotStencil, &texture_stencil);
		// "Compile" the render target.
		buffer_deferred_rt.assemble();

		// Check to make sure buffer is valid.
		if (!buffer_deferred_rt.valid())
		{
			return false;
		}

		// Create the MRT buffers
		ARCORE_ASSERT(kMRTColorAttachmentCount <= settings->colorAttachmentCount);
		for (uint i = 0; i < settings->colorAttachmentCount; ++i)
		{
			texture_deferred_color[i].allocate(core::gfx::tex::kTextureType2D, settings->colorAttachmentFormats[i], size.x, size.y, 1, 1);
		}
		buffer_deferred_color_count = settings->colorAttachmentCount;

		buffer_deferred_mrt.create(NULL);
		// Attach the color buffers for the MRT
		for (int i = 0; i < settings->colorAttachmentCount; ++i)
			buffer_deferred_mrt.attach(gpu::kRenderTargetSlotColor0 + i, &texture_deferred_color[i]);
		// Add the shared depth & stencil
		buffer_deferred_mrt.attach(gpu::kRenderTargetSlotDepth, &texture_depth);
		// Override for the combined formats (TODO: Check if this is valid)
		if (settings->depthFormat == core::gfx::tex::kDepthFormat32FStencil8 || settings->depthFormat == core::gfx::tex::kDepthFormat24Stencil8)
			buffer_deferred_mrt.attach(gpu::kRenderTargetSlotStencil, &texture_depth);
		else
			buffer_deferred_mrt.attach(gpu::kRenderTargetSlotStencil, &texture_stencil);
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
		texture_color.free();
		texture_deferred_color_composite.free();
		texture_depth.free();
		texture_stencil.free();

		for (uint i = 0; i < buffer_deferred_color_count; ++i)
			texture_deferred_color[i].free();
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