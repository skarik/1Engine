#include "core-ext/system/io/assets/TextureIO.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/gpuw/Buffers.h"
#include "renderer/gpuw/Device.h"
#include "renderer/gpuw/Fence.h"

//	StreamingReset() : Resets streaming state.
void RrTexture::StreamingReset ( void )
{
	loadState = kTextureLoadState_Verfication;
	loadLevel = 0;
}

//	virtual OnStreamStep() : Callback for streaming load.
// Arguments:
//	sync_client: True when performing a synchronized stream step.
//               When synchronized, some remote commands (GPU) will need to finish before continuing.
// Returns:
//	bool:	False when still loading, True when done.
//          The resource will stay in the "loading" list until it is done.
bool RrTexture::OnStreamStep ( bool sync_client )
{
	// move elsewhere
	gpu::Buffer pixelBuffer;
	gpu::Device* gpuq = gpu::getDevice();
	gpu::Fence pixelSync;

	switch (loadState)
	{
	case kTextureLoadState_Verfication:
		{
			// Double check parameters are correct:

			if (sync_client)
			{
				loadState = kTextureLoadState_LoadImage;
			}
			else
			{
				loadState = kTextureLoadState_LoadSuperlow;
			}
		}
		return false;

	case kTextureLoadState_LoadSuperlow:
		{
			// Create a buffer to upload the texture data
			gpu::Buffer pixelBuffer;
			pixelBuffer.allocate(sizeof(core::gfx::arPixel) * core::kTextureFormat_SuperlowSize, gpu::kTransferStatic);
			core::gfx::arPixel* target = (core::gfx::arPixel*)pixelBuffer.map(gpu::kTransferStatic);

			// Load the data in:
			core::BpdLoader loader;
			loader.m_loadSuperlow = true;
			loader.m_readTargetSuperlow = target;
			loader.LoadBpd(loadResourceFilename);

			// Unmap and upload
			pixelBuffer.unmap();
			m_texture.upload(pixelBuffer, 0);

			// Create a signal to wait on
			gpuq->sync(&pixelFence);

			// And we next want to wait on the load.
			loadState = kTextureLoadState_GPUSuperlow;
			OnStreamStep(sync_client);
		}
		return false;

	case kTextureLoadState_GPUSuperlow:
		{
			if (!pixelSync.signaled()) {
				return false;
			}
			else {
				pixelBuffer.free();
			}
			loadState = kTextureLoadState_LoadImage;
		}
		return false;

	case kTextureLoadState_UserLoading:
		{
			// Load the data in:
			core::BpdLoader loader;
			loader.m_keepMipmapsLive = true;
			loader.m_keepPaletteLive = true;
			loader.m_loadMipmapMask = 0xFF;
			loader.m_loadPalette = true;
			loader.LoadBpd(loadResourceFilename);
			// And now it's loaded!

			// We may need to do some-post work if we're using a palette:
			if (loader.palette != NULL)
			{
				// Add the palette to the global palette:
				///TODO
				// Transform input image to the global palette:
				///TODO
			}

			loadState = kTextureLoadState_NeedGPUUpload;
		}
		return false;

	case kTextureLoadState_NeedGPUUpload:
		{
			// Verify that we have all the data we need.

			loadState = kTextureLoadState_GPULoading;
			OnStreamStep(sync_client);
		}
		return false;

	case kTextureLoadState_GPULoading:
		{
			// Wait if syncing client
			if (sync_client)
			{
				//glFenceSync();
			}
			loadState = kTextureLoadState_Done;
		}
		return false;

	case kTextureLoadState_Done:
		{
			// Free our local data.
		}
		return true;
	}
}
