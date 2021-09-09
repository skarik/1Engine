#include "RrAtlasedTexture.h"
#include "core/utils/string.h"
#include "core/math/hash.h"
#include "core-ext/system/io/assets/TextureIO.h"
#include "gpuw/Buffers.h"
#include "gpuw/Device.h"

RrAtlasedTexture::RrAtlasedTexture (
	const char* s_resourceId,
	const char* s_resourcePath
)
	: RrTexture(s_resourceId, s_resourcePath)
{
	// All handled by RrTexture.

	// We need to allocate an empty texture first
	// Atlases are 2k RGBA8 textures.
	info.width = 2048;
	info.height = 2048;
	info.depth = 1;
	info.type = core::gfx::tex::kTextureType2D; 
	info.internalFormat = core::gfx::tex::kColorFormatRGBA8;
	info.levels = 1;

	// Allocate the texture.
	m_texture.allocate(info.type, info.internalFormat, info.width, info.height, info.depth, info.levels);

	// TODO: Best way would be to let the texture exist solely on the GPU. However, the GPUW layer doesn't easily allow that right now.
	// Thus, we store the entire atlas on CPU-side, and upload the entire thing every time there's a change.
	textureData = new uint32_t [info.width * info.height]; // 16 MB. This is pretty rough usage here.
	std::fill(textureData, textureData + (info.width * info.height), 0x00000000);
}

RrAtlasedTexture::~RrAtlasedTexture ( void )
{
	// Free the local texture storage
	delete[] textureData;
	textureData = NULL;
}

//	CreateEmpty ( name ) : Creates an uninitialized texture object.
// Can be used for procedural textures, with Upload(...) later.
RrAtlasedTexture*
RrAtlasedTexture::CreateEmpty ( const char* name )
{
	arstring256 resource_str_id (name);

	// We need to create a new texture:
	RrAtlasedTexture* texture = new RrAtlasedTexture(resource_str_id, NULL);

	// We don't add it to the resource system yet. For now, we just return it.

	return texture;
}

//	rrTextureAtlasLoadInfo
// Structure for current loading state
struct rrTextureAtlasLoadInfo
{
	arstring256			resourceFilename;
	//int					level = -1;
	//bool				uploading;
	//gpu::Buffer			pixelBuffer [1];
	core::BpdLoader		loader;
	//gpu::Fence			pixelSync;
	void*				data;
};

//	Add ( filename ) : Adds a texture from the disk to the atlas.
// May return information of a previously loaded instance of the texture.
const rrAtlasedTextureEntry*
RrAtlasedTexture::Add ( const char* resource_name )
{
	// Generate the resource name from the filename:
	arstring256 resource_str_id (resource_name);
	core::utils::string::ToResourceName(resource_str_id);

	// Create a hash from resource name
	core::arResourceHash hash = math::hash::fnv1a_32(resource_str_id.c_str());

	// Find the matching entry
	for (rrAtlasedTextureEntry& subtexture : subtextures)
	{
		// Check if hash matches
		if (subtexture.hash == hash &&
			// Check if resource name matches
			strcmp(subtexture.resource_name, resource_str_id) == 0)
		{
			return &subtexture;
		}
	}

	// If no entry found, we need to add a new entry
	// TODO: We want to thread this loading. For now, though, we load it in a serial manner.
	
	rrTextureAtlasLoadInfo l_loadInfo;
	rrTextureAtlasLoadInfo* loadInfo = &l_loadInfo;
	core::gfx::tex::arTextureInfo subtextureinfo;

	// Verification
	{
		// Double check parameters are correct:
		loadInfo->resourceFilename = resourceFilename;

		// Force RGBA format
		loadInfo->loader.m_forceTextureFormat = true;
		loadInfo->loader.m_forcedTextureFormat = core::gfx::tex::kColorFormatRGBA8;

		// Hit the LoadBpd to verify the resource converted and file exists:
		loadInfo->loader.m_loadImageInfo = true;
		auto loadStatus = loadInfo->loader.LoadBpd(loadInfo->resourceFilename); // open bpd now
		loadInfo->loader.m_loadImageInfo = false;
		ARCORE_ASSERT(loadStatus);

		// Set up the image info for allocating data down the line:
		// BPDs default to 2D 8-bit RGBA for now.
		subtextureinfo.type = core::gfx::tex::kTextureType2D; 
		subtextureinfo.internalFormat = core::gfx::tex::kColorFormatRGBA8;
		// The rest of the information will be available in the loader.
		subtextureinfo.width = loadInfo->loader.info.width;
		subtextureinfo.height = loadInfo->loader.info.height;
		subtextureinfo.depth = loadInfo->loader.info.depth;
		subtextureinfo.levels = loadInfo->loader.info.levels;

		// Update the format
		subtextureinfo.internalFormat = core::getColorFormatFromTextureFormat(loadInfo->loader.format);

		// Create the sync
		//loadInfo->pixelSync.create(NULL);

		// Allocate the texture.
		//m_texture.allocate(info.type, info.internalFormat, info.width, info.height, info.depth, info.levels);
	}
	// Skip superlow
	{}
	// Load image
	{
		// Load in highest mip of the image

		// Create a buffer to upload the texture data
		uint16_t level_width	= std::max<uint16_t>(1, loadInfo->loader.info.width);
		uint16_t level_height	= std::max<uint16_t>(1, loadInfo->loader.info.height);
		uint16_t level_depth	= std::max<uint16_t>(1, loadInfo->loader.info.depth);

		//loadInfo->pixelBuffer[0].initAsTextureBuffer(NULL, info.type, info.internalFormat, level_width, level_height, level_depth);
		//void* target = loadInfo->pixelBuffer[0].map(NULL, gpu::kTransferWriteDiscardPrevious);
		loadInfo->data = new uint32_t[level_width * level_height];
		void* target = loadInfo->data;

		// Load the data in
		loadInfo->loader.m_buffer_Mipmaps[0] = target;
		loadInfo->loader.m_loadMipmapMask = 0x01 << 0;
		loadInfo->loader.LoadBpd();
		loadInfo->loader.m_buffer_Mipmaps[0] = NULL;

		// Unmap and upload
		//loadInfo->pixelBuffer[0].unmap(NULL);
		//m_texture.upload(loadInfo->pixelBuffer[0], 0, 0);
	}

	// Find position for new entry
	{
		rrAtlasedTextureEntry new_subtexture;
		new_subtexture.resource_name = resource_str_id;
		new_subtexture.hash = hash;
		new_subtexture.pixel_size.x = loadInfo->loader.info.width;
		new_subtexture.pixel_size.y = loadInfo->loader.info.height;

		// now find new position
		// loop through each Y, taking a minimum Y greater than current Y. If we cant find something at the current Y, then we go to the next minimum Y.
		Vector2i current_position (0, 0);
		int32_t next_minimum_y = 0;
		bool has_found_position = false;
		while (!has_found_position)
		{
			has_found_position = true;

			// Loop through all the bounding boxes, looking for boxes in the Y range
			for (const auto& subtexture : subtextures)
			{
				if (subtexture.pixel_position.y + subtexture.pixel_size.y > current_position.y
					&& subtexture.pixel_position.y < current_position.y + (int32_t)subtextureinfo.height)
				{
					// in range of y, check if colliding with current x
					if (subtexture.pixel_position.x + subtexture.pixel_size.x > current_position.x
						&& subtexture.pixel_position.x < current_position.x + (int32_t)subtextureinfo.width)
					{
						// Position not free, we go to next position
						has_found_position = false;
						current_position.x = std::max<int32_t>(subtexture.pixel_position.x + subtexture.pixel_size.x, current_position.x);

						// Update the next Y to check at when we go to the next row
						if (next_minimum_y == current_position.y || subtexture.pixel_position.y + subtexture.pixel_size.y < next_minimum_y)
						{
							next_minimum_y = subtexture.pixel_position.y + subtexture.pixel_size.y;
						}
						
						// Stop checking collision, check through everything again
						break;
					}
				}
			}

			// If found position, check that we're in range
			if (has_found_position)
			{
				if (current_position.x + subtextureinfo.width > info.width)
				{
					// Not in range, go to next row and continue
					has_found_position = false;
					current_position.x = 0;
					current_position.y = next_minimum_y;
					continue;
				}
				else
				{
					// Valid position, save this.
					has_found_position = true;
					new_subtexture.pixel_position = current_position;
					new_subtexture.pixel_size = Vector2i(subtextureinfo.width, subtextureinfo.height);
				}
			}
		}

		// position found, copy it in line-by-line
		uint32_t* input_data = (uint32_t*)loadInfo->data;
		uint32_t* output_data = (uint32_t*)textureData;
		for (int32_t iy = 0; iy < new_subtexture.pixel_size.y; ++iy)
		{
			std::copy(
				input_data + iy * new_subtexture.pixel_size.x, 
				input_data + iy * (new_subtexture.pixel_size.x + 1),
				output_data + new_subtexture.pixel_position.x + (new_subtexture.pixel_position.y + iy) * info.width);
		}
	}

	// cleanup. done with data
	{
		delete[] loadInfo->data;
	}

	// Upload final
	{
		this->Upload(true, textureData, info.width, info.height, info.internalFormat, info.repeatX, info.repeatY, core::gfx::tex::kMipmapGenerationNone, info.filter);
	}


	// Otherwise, failed to add new entry, return failure
	return NULL;
}
