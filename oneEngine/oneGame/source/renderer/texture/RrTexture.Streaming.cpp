#include "core/math/Math.h"
#include "core-ext/system/io/assets/TextureIO.h"
#include "core-ext/threads/Jobs.h"
#include "renderer/texture/RrTexture.h"
#include "gpuw/Buffers.h"
#include "gpuw/Device.h"
#include "gpuw/Fence.h"

enum rrTextureUnpackState
{
	kTextureUnpack0_MapAndDispatch = 0,
	kTextureUnpack1_WaitForDispatch = 1,
	kTextureUnpack2_Upload = 2,
};

//	rrTextureLoadInfo
// Structure for current loading state
struct rrTextureLoadInfo
{
	arstring256			resourceFilename;
	int					level;
	std::atomic<rrTextureUnpackState>
						unpack_step;
	bool				uploading;
	gpu::Buffer			pixelBuffer [16];
	core::BpdLoader		loader;
	gpu::Fence			pixelSync;
};

//	StreamingReset() : Resets streaming state.
void RrTexture::StreamingReset ( void )
{
	if (loadInfo != NULL)
	{	// Clear off load info state and dirty values.
		loadInfo->pixelSync.destroy(NULL);
		for (int i = 0; i < 16; ++i) {
			loadInfo->pixelBuffer[i].free(NULL);
		}
		delete loadInfo;
	}

	loadInfo = new rrTextureLoadInfo;

	loadState = kTextureLoadState_Verfication;
	loadInfo->level = -1;
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
	// Streaming load of procedural textures
	if (procedural && upload_request != NULL)
	{
		return OnStreamStepProcedural(sync_client);
	}
	// Streaming load of textures
	else if (!procedural)
	{
		return OnStreamStepDisk(sync_client);
	}
	//
	// Catch-all case
	else 
	{
		return true;
	}
}

bool RrTexture::OnStreamStepProcedural ( bool sync_client )
{
	gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	switch (loadState)
	{
	case kTextureLoadState_Verfication:
		{
			// Create the sync
			loadInfo->pixelSync.create(NULL);

			// Set up the image info for procedural textures:
			info.type = upload_request->type;
			info.width = upload_request->width;
			info.height = upload_request->height;
			info.depth = upload_request->depth;
			info.internalFormat = upload_request->format;
			info.levels = 1;

			// Go to next state
			loadState = kTextureLoadState_LoadImage;
		}
		return false;

	case kTextureLoadState_LoadImage:
		{
			// Create a buffer to upload the texture data
			loadInfo->pixelBuffer[0].initAsTextureBuffer(NULL, upload_request->type, upload_request->format, upload_request->width, upload_request->height, upload_request->depth);
			void* target = loadInfo->pixelBuffer[0].map(NULL, gpu::kTransferStatic);

			// Copy data to the target
			if (upload_request->data != NULL)
			{
				memcpy(target, upload_request->data, core::gfx::tex::getColorFormatByteSize(upload_request->format) * upload_request->width * upload_request->height);
			}

			// Unmap and upload
			loadInfo->pixelBuffer[0].unmap(NULL);
			m_texture.allocate(info.type, info.internalFormat, info.width, info.height, info.depth, info.levels);
			m_texture.upload(loadInfo->pixelBuffer[0], 0, 0);

			// Create a signal to wait on
			gfx->signal(&loadInfo->pixelSync);

			// And we next want to wait on the load.
			loadState = kTextureLoadState_GPUImage;
			return OnStreamStep(sync_client);
		}
		return false;

	case kTextureLoadState_GPUImage:
		{
			if (!loadInfo->pixelSync.signaled()) {
				return false;
			}
			else {
				loadInfo->pixelBuffer[0].free(NULL);
			}
			loadState = kTextureLoadState_Done;
		}
		return false;

	case kTextureLoadState_Done:
		{
			// Free our local data.
			if (loadInfo != NULL)
			{
				loadInfo->pixelSync.destroy(NULL);
				delete loadInfo;
			}
			if (upload_request != NULL)
			{
				delete upload_request;
				upload_request = NULL;
			}
		}
		return true;

	default:
		return true;
	}
}

bool RrTexture::OnStreamStepDisk ( bool sync_client )
{
	gpu::GraphicsContext* gfx = gpu::getDevice()->getContext();

	switch (loadState)
	{
	case kTextureLoadState_Verfication:
		{
			// Double check parameters are correct:
			loadInfo->resourceFilename = resourceFilename;

			// Hit the LoadBpd to verify the resource converted and file exists:
			loadInfo->loader.m_loadImageInfo = true;
			auto loadStatus = loadInfo->loader.LoadBpd(loadInfo->resourceFilename); // open bpd now
			loadInfo->loader.m_loadImageInfo = false;
			ARCORE_ASSERT(loadStatus);

			// Set up the image info for allocating data down the line:
			// BPDs default to 2D 8-bit RGBA for now.
			info.type = core::gfx::tex::kTextureType2D; 
			info.internalFormat = core::gfx::tex::kColorFormatRGBA8;
			// The rest of the information will be available in the loader.
			info.width = loadInfo->loader.info.width;
			info.height = loadInfo->loader.info.height;
			info.depth = loadInfo->loader.info.depth;
			info.levels = loadInfo->loader.info.levels;

			// Update the format
			info.internalFormat = core::getColorFormatFromTextureFormat(loadInfo->loader.format);

			// Create the sync
			loadInfo->pixelSync.create(NULL);

			// Allocate the texture.
			m_texture.allocate(info.type, info.internalFormat, info.width, info.height, info.depth, info.levels);

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
			uint64_t size_Flat = core::kTextureFormat_SuperlowByteSize / core::gfx::tex::getColorFormatByteSize(info.internalFormat);
			uint64_t size_Width = (uint64_t)sqrt(size_Flat);
			loadInfo->pixelBuffer[0].initAsTextureBuffer(NULL, info.type, info.internalFormat, size_Width, size_Width, 1); // TODO: Take format into account.
			void* target = loadInfo->pixelBuffer[0].map(NULL, gpu::kTransferStatic);

			// Load the data in:
			loadInfo->loader.m_buffer_Superlow = target; // Target set here
			loadInfo->loader.m_loadOnlySuperlow = true;
			loadInfo->loader.LoadBpd();
			loadInfo->loader.m_buffer_Superlow = NULL;
			loadInfo->loader.m_loadOnlySuperlow = false;

			// Unmap and upload
			loadInfo->pixelBuffer[0].unmap(NULL);
			m_texture.upload(loadInfo->pixelBuffer[0], info.levels - 1, 0);

			// Create a signal to wait on
			gfx->signal(&loadInfo->pixelSync);

			// And we next want to wait on the load.
			loadState = kTextureLoadState_GPUSuperlow;
			return OnStreamStep(sync_client);
		}
		return false;

	case kTextureLoadState_GPUSuperlow:
		{
			if (!loadInfo->pixelSync.signaled()) {
				return false;
			}
			else {
				loadInfo->pixelBuffer[0].free(NULL);
			}
			loadState = kTextureLoadState_LoadImage;
		}
		return false;

	case kTextureLoadState_LoadImage:
		{
			// Loader isn't set up? Set it up.
			if (loadInfo->level == -1)
			{
				// Load the data in:
				loadInfo->loader.m_loadImageInfo = true;
				loadInfo->loader.LoadBpd();
				loadInfo->loader.m_loadImageInfo = false;

				loadInfo->level = loadInfo->loader.info.levels - 1;
				loadInfo->uploading = false;
				loadInfo->unpack_step = kTextureUnpack0_MapAndDispatch;
			}
			else if (!loadInfo->uploading)
			{
				if (loadInfo->unpack_step == kTextureUnpack0_MapAndDispatch)
				{
					// Create a buffer to upload the texture data
					uint16_t level_width	= std::max<uint16_t>(1, loadInfo->loader.info.width / math::exp2(loadInfo->level));
					uint16_t level_height	= std::max<uint16_t>(1, loadInfo->loader.info.height / math::exp2(loadInfo->level));
					uint16_t level_depth	= std::max<uint16_t>(1, loadInfo->loader.info.depth / math::exp2(loadInfo->level));

					loadInfo->pixelBuffer[loadInfo->level].initAsTextureBuffer(NULL, info.type, info.internalFormat, level_width, level_height, level_depth);
					void* target = loadInfo->pixelBuffer[loadInfo->level].map(NULL, gpu::kTransferStatic);

					// Go to the waiting step before we dispatch
					loadInfo->unpack_step = kTextureUnpack1_WaitForDispatch;

					auto uploadJobId = core::jobs::System::Current::AddJobRequest([this, target]()
					{
						// Load the data in
						loadInfo->loader.m_buffer_Mipmaps[loadInfo->level] = target;
						loadInfo->loader.m_loadMipmapMask = 0x01 << loadInfo->level;
						loadInfo->loader.LoadBpd();
						loadInfo->loader.m_buffer_Mipmaps[loadInfo->level] = NULL;

						// We're done. Go to the upload step.
						loadInfo->unpack_step = kTextureUnpack2_Upload;
					});
				}

				if (loadInfo->unpack_step == kTextureUnpack2_Upload)
				{
					// Unmap and upload
					loadInfo->pixelBuffer[loadInfo->level].unmap(NULL);
					m_texture.upload(loadInfo->pixelBuffer[loadInfo->level], loadInfo->level, 0);

					// If we're not forcing sync, we need to wait until we're done uploading that texture level.
					if (!sync_client)
					{
						// Create a signal to wait on
						loadInfo->pixelSync.init();
						gfx->signal(&loadInfo->pixelSync);
						loadInfo->uploading = true;
						return OnStreamStep(sync_client);
					}
					// If we're forcing sync, we can immediately go to waiting on the texture.
					else
					{
						// Let's go back to the first step
						loadInfo->unpack_step = kTextureUnpack0_MapAndDispatch;

						// Decrement the level:
						loadInfo->level -= 1;
						if (loadInfo->level == -1) // Finished the last level...
						{
							// Create signal to wait on
							loadInfo->pixelSync.init();
							gfx->signal(&loadInfo->pixelSync);
							loadInfo->uploading = true; 
							// Mark done here
							loadState = kTextureLoadState_GPUImage;
						}
						return false;
					}
				}

				return false;
			}
			// We're uploading and so we should wait on the GPU.
			else
			{
				if (!loadInfo->pixelSync.signaled()) {
					return false;
				}
				else
				{	// Done uploading to GPU. We can free the previous buffer.
					loadInfo->pixelBuffer[loadInfo->level].free(NULL);

					// Mark that we're done uploading
					loadInfo->uploading = false;
					loadInfo->unpack_step = kTextureUnpack0_MapAndDispatch;

					// Decrement the level:
					loadInfo->level -= 1;
					if (loadInfo->level == -1) // Finished the last level...
					{
						loadState = kTextureLoadState_GPUImage; // Already sync'd, so we can keep going.
					}

					return false;
				}
			}
		}
		return false;

	case kTextureLoadState_GPUImage:
		{
			// Wait if uploading
			if (loadInfo->uploading)
			{
				if (!loadInfo->pixelSync.signaled()) {
					return false;
				}
				else
				{
					// Uploading is done, we can now free all of our used pixel buffers.
					loadInfo->uploading = false;
					for (int i = 0; i < loadInfo->loader.info.levels; ++i) {
						loadInfo->pixelBuffer[i].free(NULL);
					}
				}
			}
			else
			{
				// Verify that we have all the data we need.

				// Mark done.
				loadState = kTextureLoadState_Done;
				return OnStreamStep(sync_client);
			}
		}
		return false;

	case kTextureLoadState_Done:
		{
			// Free our local data.
			if (loadInfo != NULL)
			{
				loadInfo->pixelSync.destroy(NULL);
				delete loadInfo;
			}
		}
		return true;

	default:
		return true;
	}
}