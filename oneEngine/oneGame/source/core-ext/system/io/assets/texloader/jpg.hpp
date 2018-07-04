#ifndef CORE_ASSETS_TEXLOADER_JPG_HPP_
#define CORE_ASSETS_TEXLOADER_JPG_HPP_

#include <string>
#include <stdint.h>

#include "core/gfx/pixelFormat.h"
#include "core/gfx/textureStructures.h"

#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"

namespace core {
namespace texture
{
	static gfx::arPixel* loadJPG ( const std::string& n_inputfile, gfx::tex::arImageInfo& o_info )
	{
		int calcW, calcH, calcBPP;
		unsigned char * pixelData = NULL;
		gfx::arPixel* pData = NULL;

		// Read in the data
		FILE* file = fopen( n_inputfile.c_str(), "rb");  //open the file

		// Structures needed for info
		struct jpeg_decompress_struct info_jpg; 
		struct jpeg_error_mgr err;

		info_jpg.err = jpeg_std_error(&err);     //tell the jpeg decompression handler to send the errors to err
		jpeg_create_decompress(&info_jpg);       //sets info to all the default stuff

		if ( !file )
		{
			pixelData = NULL; // Mark as null if no file opened
		}
		else
		{
			// tell the jpeg lib the file we'er reading
			jpeg_stdio_src(&info_jpg, file);    
			//tell it to start reading it
			jpeg_read_header(&info_jpg, TRUE);   

			//read it fast!
			info_jpg.do_fancy_upsampling = FALSE; 
			//decompress the file
			jpeg_start_decompress(&info_jpg);    

			//set the x and y size
			calcW = info_jpg.output_width;
			calcH = info_jpg.output_height;
			calcBPP = info_jpg.num_components;

			// Create input data
			pixelData = new unsigned char [ calcW * calcH * calcBPP ];

			unsigned char* p1 = pixelData;
			unsigned char** p2 = &p1;
			int numlines = 0;

			while(info_jpg.output_scanline < info_jpg.output_height)
			{
				numlines = jpeg_read_scanlines(&info_jpg, p2, 1);
				*p2 += numlines * calcBPP * info_jpg.output_width;
			}

			// finish decompressing this file
			jpeg_finish_decompress(&info_jpg);  
			// free info
			jpeg_destroy_decompress(&info_jpg); 
			// close the file
			fclose(file); 
		}

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
			// Return the target data
			return pData;
		}
		else
		{
			// Print error if data is invalid
			fprintf( stderr, "Could not read JPG file \"%s\"\n", n_inputfile.c_str() );
			// Return no data read
			return NULL;
		}
	}
}}

#endif//CORE_ASSETS_TEXLOADER_JPG_HPP_