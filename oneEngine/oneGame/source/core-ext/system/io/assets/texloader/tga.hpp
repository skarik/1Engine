#ifndef CORE_ASSETS_TEXLOADER_TGA_HPP_
#define CORE_ASSETS_TEXLOADER_TGA_HPP_

#include <string>
#include <stdint.h>

#include "core/gfx/pixelFormat.h"
#include "core/gfx/textureStructures.h"

#include "tga/tga_loader.h"

namespace core {
namespace texture
{
	static gfx::arPixel* loadTGA ( const char* const n_inputfile, gfx::tex::arImageInfo& o_info )
	{
		int calcW, calcH, calcBPP;
		unsigned char * pixelData = NULL;
		gfx::arPixel* pData = NULL;

		// Read in the data
		pixelData = RGB_TGA::rgb_tga( n_inputfile, &calcW, &calcH, &calcBPP );

		// If the data is valid
		if ( pixelData != NULL )
		{
			// Set the new texture size
			o_info.width = calcW;
			o_info.height = calcH;
			o_info.depth = 1;

			// Check and set the bitdepth
			/*if ( calcBPP == 3 )
				o_info.internalFormat = gfx::tex::kColorFormatRGB8;
			else if ( calcBPP == 4 )
				o_info.internalFormat = gfx::tex::kColorFormatRGBA8;*/
			// Create the pixel data
			pData = new gfx::arPixel [ o_info.width * o_info.height ];

			// Go through the stored data and save it to the texture pixel data
			unsigned int iTarget;
			unsigned int iSource;
			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
				{
					iTarget = ix+(iy*o_info.width);
					iSource = (ix+((o_info.height-iy-1)*o_info.width))*calcBPP;
					pData[ iTarget ].r = pixelData[ iSource+0 ];
					pData[ iTarget ].g = pixelData[ iSource+1 ];
					pData[ iTarget ].b = pixelData[ iSource+2 ];
					if ( calcBPP == 4 ) {
						pData[ iTarget ].a = pixelData[ iSource+3 ];
					}
					else {
						pData[ iTarget ].a = 255;
					}
				}
			}

			// Delete the source data
			delete [] pixelData;
			// Return final data
			return pData;
		}
		else
		{
			// Print error if data is invalid
			fprintf( stderr, "Could not read TGA file \"%s\"\n", n_inputfile );
			// Return no data read
			return NULL;
		}
	}
}}

#endif//CORE_TEXLOADER_TGA_HPP_