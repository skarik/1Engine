#ifndef CORE_ASSETS_TEXLOADER_BMP_HPP_
#define CORE_ASSETS_TEXLOADER_BMP_HPP_

#include <string>
#include <stdint.h>

#include "core/gfx/pixelFormat.h"
#include "core/gfx/textureStructures.h"

#include "stb/stb_image.h"

namespace core {
namespace texture
{
	static gfx::arPixel* loadBMP ( const std::string& n_inputfile, gfx::tex::arImageInfo& o_info )
	{
		int calcW, calcH, calcBPP;
		unsigned char * pixelData = NULL;
		gfx::arPixel* pData = NULL;

		// Read in the data
		pixelData = stbi_load( n_inputfile.c_str(), &calcW, &calcH, &calcBPP, 4 );

		// If the data is valid
		if ( pixelData != NULL )
		{
			// Set the new texture size
			o_info.width = calcW;
			o_info.height = calcH;

			// Check and set the bitdepth
			if ( calcBPP == 3 )
				o_info.internalFormat = gfx::tex::kColorFormatRGB8;
			else if ( calcBPP == 4 )
				o_info.internalFormat = gfx::tex::kColorFormatRGBA8;
			// Create the pixel data
			pData = new gfx::arPixel [ o_info.width * o_info.height ];

			// Go through the stored data and save it to the texture pixel data
			unsigned int iTarget;
			unsigned int iSource;
			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
				{
					iTarget = ix+((o_info.height-iy-1)*o_info.width);
					iSource = (ix+((o_info.height-iy-1)*o_info.width))*calcBPP;
					pData[ iTarget ].r = pixelData[ iSource+2 ];
					pData[ iTarget ].g = pixelData[ iSource+1 ];
					pData[ iTarget ].b = pixelData[ iSource+0 ];
					if ( calcBPP == 4 ) {
						pData[ iTarget ].a = pixelData[ iSource+3 ];
					}
					else {
						pData[ iTarget ].a = 255;
					}
				}
			}

			// Delete the source data
			stbi_image_free(pixelData);
			// Return final data
			return pData;
		}
		else
		{
			// Print error if data is invalid
			fprintf( stderr, "Could not read BMP file \"%s\"\n", n_inputfile.c_str() );
			// Return no data read
			return NULL;
		}
	}
}}

#endif//CORE_ASSETS_TEXLOADER_BMP_HPP_