
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

	// Get the background color
	LONG bg_color_composite = ggGetFrameInfo( t_file, 0, 1 );
	uint8_t* bg_color = (uint8_t*)&bg_color_composite;

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

				if (image[dest].r == bg_color[0] && image[dest].g == bg_color[1] && image[dest].b == bg_color[2])
					image[dest].a = 0;
				else
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

	// Get the background color
	LONG bg_color_composite = ggGetFrameInfo( t_file, 0, 1 );
	uint8_t* bg_color = (uint8_t*)&bg_color_composite;

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

				if (image[dest].r == bg_color[0] && image[dest].g == bg_color[1] && image[dest].b == bg_color[2])
					image[dest].a = 0;
				else
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


// load animation
pixel_t* Textures::loadGAL_Animation ( const std::string& n_inputfile, timgInfo& o_info, Real** o_frametimes )
{
#ifndef _ENGINE_RELEASE

	// Open the GG file
	void* t_file = ggOpen( n_inputfile.c_str() );

	// Get the frame count
	DWORD frame_count = ggGetFrameCount( t_file );
	HBITMAP hBitmap;
	BITMAP oBitmap;

	// Get the background color
	LONG bg_color_composite = ggGetFrameInfo( t_file, 0, 1 );
	uint8_t* bg_color = (uint8_t*)&bg_color_composite;

	// Get a combined bitmap from the GAL file
	hBitmap = ggGetBitmap(t_file, 0, -1);
	if (hBitmap == NULL)
	{
		return NULL;
	}
	oBitmap;
	if (GetObject( hBitmap, sizeof(BITMAP), &oBitmap ) == NIL)
	{
		return NULL;
	}

	// Use that to set its size
	uint16_t frame_width	= (uint16_t)oBitmap.bmWidth;
	uint16_t frame_height	= (uint16_t)oBitmap.bmHeight;
	o_info.width	= frame_width;
	o_info.height	= frame_height;
	o_info.internalFormat	= RGBA8;
	// Use the frame count to generate full animation size
	o_info.framecount	= (uint8_t)frame_count;
	o_info.xdivs		= (uint8_t)frame_count;
	o_info.ydivs		= 1;
	// Even number of frames? Do normal cutting.
	if ( frame_count % 2 == 0 )
	{
		while ( (o_info.xdivs % 2 == 0) && (o_info.ydivs <= o_info.xdivs / 2) )
		{
			o_info.xdivs /= 2;
			o_info.ydivs *= 2;
		}
	}
	// Odd number of frames? Do wierd shit.
	if ( frame_count % 2 == 1 )
	{
		while ( (o_info.ydivs <= o_info.xdivs / 3) )
		{
			o_info.xdivs = o_info.xdivs / 2;
			o_info.ydivs = o_info.ydivs * 2 + 1;

			// Fit the new size to the actual frame count
			while ( ((int16_t)o_info.xdivs) * ((int16_t)o_info.ydivs) > (int16_t)frame_count )
			{
				o_info.xdivs -= 1;
			}
			while ( ((int16_t)o_info.xdivs) * ((int16_t)o_info.ydivs) < (int16_t)frame_count )
			{	// This takes care of going too low in the above looop
				o_info.xdivs += 1;
			}
		}
	}

	// Create full image width from that
	o_info.width *= o_info.xdivs;
	o_info.height *= o_info.ydivs;

	// Create frametimes result data
	if ( o_frametimes != NULL )
	{
		*o_frametimes = new Real[frame_count];
	}

	// Create the image result image data
	pixel_t* image = new pixel_t[o_info.width * o_info.height];
	int depth = oBitmap.bmBitsPixel / 8;

	// Loop through each frame...
	for ( uint frame = 0; frame < frame_count; ++frame )
	{
		// Get frame time
		if ( o_frametimes != NULL )
		{
			(*o_frametimes)[frame] = ggGetFrameInfo(t_file, frame, 2) / 1000.0F;
		}

		// Get a combined bitmap from the GAL file
		hBitmap = ggGetBitmap(t_file, frame, -1);
		if (hBitmap == NULL)
		{
			return NULL;
		}
		oBitmap;
		if (GetObject( hBitmap, sizeof(BITMAP), &oBitmap ) == NIL)
		{
			return NULL;
		}

		// Create the image result image data
		for ( uint y = 0; y < frame_height; ++y )
		{
			for ( uint x = 0; x < frame_width; ++x )
			{
				int source_offset = x * depth + (frame_height - y - 1) * oBitmap.bmWidthBytes;
				int dest =
					(x + frame_width * (frame % o_info.xdivs)) + 
					(y + frame_height * (frame / o_info.xdivs)) * o_info.width;
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
					
					if (image[dest].r == bg_color[0] && image[dest].g == bg_color[1] && image[dest].b == bg_color[2])
						image[dest].a = 0;
					else
						image[dest].a = 255;
				}
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