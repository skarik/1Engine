
#include "renderer/types/textureFormats.h"
#include "render2d/texture/TextureLoader.h"

#ifndef _ENGINE_RELEASE
#include "gale/galefile.h"
#endif//_ENGINE_RELEASE

pixel_t* Textures::loadGAL ( const std::string& n_inputfile, timgInfo& o_info )
{
#ifndef _ENGINE_RELEASE

	// Open the GG file
	void* t_file = ggOpen( n_inputfile.c_str() );

	// Get the combined bitmap from the GAL file
	HBITMAP hBitmap = ggGetBitmap(t_file, 0, -1);
	if (hBitmap == NULL)
	{
		return NULL;
	}
	BITMAP oBitmap;
	if (GetObject( hBitmap, sizeof(BITMAP), &oBitmap ) == NIL)
	{
		return NULL;
	}

	// Set image sizes
	o_info.width	= (uint16_t)oBitmap.bmWidth;
	o_info.height	= (uint16_t)oBitmap.bmHeight;
	o_info.internalFormat	= RGBA8;

	// Create the image result image data
	pixel_t* image = new pixel_t[o_info.width * o_info.height];
	int depth = oBitmap.bmBitsPixel / 8;
	for ( uint y = 0; y < o_info.height; ++y )
	{
		for ( uint x = 0; x < o_info.width; ++x )
		{
			int source_offset = x * depth + (o_info.height - y - 1) * oBitmap.bmWidthBytes;
			int dest = x + y * o_info.width;
			if (depth == 4)
			{
				image[dest].r = *(((uchar*)oBitmap.bmBits) + source_offset + 3);
				image[dest].g = *(((uchar*)oBitmap.bmBits) + source_offset + 2);
				image[dest].b = *(((uchar*)oBitmap.bmBits) + source_offset + 1);
				image[dest].a = *(((uchar*)oBitmap.bmBits) + source_offset + 0);
			}
			else
			{
				image[dest].r = *(((uchar*)oBitmap.bmBits) + source_offset + 2);
				image[dest].g = *(((uchar*)oBitmap.bmBits) + source_offset + 1);
				image[dest].b = *(((uchar*)oBitmap.bmBits) + source_offset + 0);
				image[dest].a = 255;
			}
		}
	}

	// Close
	ggClose( t_file );

	// Return result data that was loaded
	return image;

#else
	return NULL;
#endif
}


pixel_t* Textures::loadGAL_Layer ( const std::string& n_inputfile, const std::string& n_layername, timgInfo& o_info )
{
#ifndef _ENGINE_RELEASE

	// Open the GG file
	void* t_file = ggOpen( n_inputfile.c_str() );

	// Get the combined bitmap from the GAL file
	int target_layer = -1;
	{
		char name [1024];
		int layercount = ggGetLayerCount(t_file, 0);
		for (int i = 0; i < layercount; ++i)
		{
			ggGetLayerName(t_file, 0, i, name, 1024);
			if ( n_layername == name )
			{
				target_layer = i;
				break;
			}
		}
	}
	if (target_layer == -1)
	{
		return NULL;
	}
	HBITMAP hBitmap = ggGetBitmap(t_file, 0, target_layer);
	if (hBitmap == NULL)
	{
		return NULL;
	}
	BITMAP oBitmap;
	if (GetObject( hBitmap, sizeof(BITMAP), &oBitmap ) == NIL)
	{
		return NULL;
	}

	// Set image sizes
	o_info.width	= (uint16_t)oBitmap.bmWidth;
	o_info.height	= (uint16_t)oBitmap.bmHeight;
	o_info.internalFormat	= RGBA8;

	// Create the image result image data
	pixel_t* image = new pixel_t[o_info.width * o_info.height];
	int depth = oBitmap.bmBitsPixel / 8;
	for ( uint y = 0; y < o_info.height; ++y )
	{
		for ( uint x = 0; x < o_info.width; ++x )
		{
			int source_offset = x * depth + (o_info.height - y - 1) * oBitmap.bmWidthBytes;
			int dest = x + y * o_info.width;
			if (depth == 4)
			{
				image[dest].r = *(((uchar*)oBitmap.bmBits) + source_offset + 3);
				image[dest].g = *(((uchar*)oBitmap.bmBits) + source_offset + 2);
				image[dest].b = *(((uchar*)oBitmap.bmBits) + source_offset + 1);
				image[dest].a = *(((uchar*)oBitmap.bmBits) + source_offset + 0);
			}
			else
			{
				image[dest].r = *(((uchar*)oBitmap.bmBits) + source_offset + 2);
				image[dest].g = *(((uchar*)oBitmap.bmBits) + source_offset + 1);
				image[dest].b = *(((uchar*)oBitmap.bmBits) + source_offset + 0);
				image[dest].a = 255;
			}
		}
	}

	// Close
	ggClose( t_file );

	// Return result data that was loaded
	return image;

#else
	return NULL;
#endif
}
