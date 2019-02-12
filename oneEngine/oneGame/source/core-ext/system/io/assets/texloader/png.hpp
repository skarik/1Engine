#ifndef CORE_ASSETS_TEXLOADER_PNG_HPP_
#define CORE_ASSETS_TEXLOADER_PNG_HPP_

#include <string>
#include <stdint.h>

#include "core/gfx/pixelFormat.h"
#include "core/gfx/textureStructures.h"
#include "core-ext/system/io/assets/TextureIO.h"

#include "png/png.h"

namespace core {
namespace texture
{
	static gfx::arPixel* loadPNG ( const char* const n_inputfile, gfx::tex::arImageInfo& o_info )
	{
		int calcW, calcH, calcBPP;
		unsigned char * pixelData = NULL;
		gfx::arPixel* pData = NULL;

		ETextureLoadMode alphaMode = ALPHA_LOAD_MODE_DEFAULT;
		int iTransSize = 0;
		gfx::arPixel* pTransTable = NULL;

		// Read in the data
		FILE* file = fopen( n_inputfile, "rb");  //open the file

		if ( !file )
		{
			fprintf( stderr, "Could not open PNG file \"%s\"\n", n_inputfile );
			pixelData = NULL;
		}
		else
		{
			unsigned char header[8];    // 8 is the maximum size that can be checked
			fread( header, 1, 8, file );
			if ( png_sig_cmp( header, 0, 8 ) )
				fprintf( stderr, "File %s is not recognized as a PNG file", n_inputfile );

			// Define needed vars
			png_structp	png_ptr;
			png_infop	info_ptr;
			png_infop	end_info;

			// Init stuff
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

			if (!png_ptr) {
				fprintf( stderr, "[loadPNG] png_create_read_struct failed" );
			}

			info_ptr = png_create_info_struct( png_ptr );

			if (!info_ptr) {
				png_destroy_read_struct( &png_ptr, NULL, NULL );
				fprintf( stderr, "[loadPNG] png_create_info_struct failed" );
			}
			if (setjmp(png_jmpbuf(png_ptr))) {
				fprintf( stderr, "[loadPNG] Error during init_io" );
			}

			end_info = png_create_info_struct( png_ptr );
			if (!end_info) {
				png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
				fprintf( stderr, "[loadPNG] png_create_info_struct failed for end_info" );
			}

			// Now, begin the reading
			png_init_io( png_ptr, file );
			png_set_sig_bytes( png_ptr, 8 );

			png_read_info( png_ptr, info_ptr );

			calcW = png_get_image_width(png_ptr, info_ptr);
			calcH = png_get_image_height(png_ptr, info_ptr);

			// Figure out the bit depth ajd transparency modes
			int color_type, bit_depth;
			png_bytep trans_alpha = NULL;
			int num_trans = 0;
			png_color_16p trans_color = NULL;

			color_type = png_get_color_type(png_ptr, info_ptr);
			bit_depth = png_get_bit_depth(png_ptr, info_ptr);
			if ( color_type == PNG_COLOR_TYPE_RGBA ) {
				calcBPP = (bit_depth / 8)*4;
			}
			else if ( color_type == PNG_COLOR_TYPE_RGB ) {
				calcBPP = (bit_depth / 8)*3;
				// Check for tRNS alpha chunk
				{
					png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, &trans_color);
					if (trans_alpha != NULL)
					{
						alphaMode = ALPHA_LOAD_MODE_INDEXED;
					}
					else if ( num_trans >= 1 )
					{
						iTransSize = 1;
						pTransTable = new gfx::arPixel [1];
						pTransTable->r = trans_color->red;
						pTransTable->g = trans_color->green;
						pTransTable->b = trans_color->blue;
						pTransTable->a = trans_color->gray;
						alphaMode = ALPHA_LOAD_MODE_KEYED;
					}
				}
			}
			else { // todo: color_type == PNG_COLOR_TYPE_GA
				calcBPP = 1;
			}
			//cout << "PNG BPP: " << calcBPP << endl;

			int number_of_passes;
			number_of_passes = png_set_interlace_handling(png_ptr);
			png_read_update_info(png_ptr, info_ptr);

			// Read the file
			if (setjmp(png_jmpbuf(png_ptr))) {
				fprintf( stderr, "[loadPNG] Error during read_image" );
			}

			// Create input data
			pixelData = new (std::nothrow) unsigned char [ calcW * calcH * calcBPP ];

			if ( pixelData ) 
			{
				// Input data to file
				uint32_t rowbytes;
				void* raw_image;
				png_bytep * row_pointers;
				row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * calcH);
				//for ( int y = 0; y < calcH; y++ )
				//	row_pointers[y] = png_malloc( png_ptr, 
				//        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
				rowbytes = png_get_rowbytes(png_ptr,info_ptr);
				raw_image = (png_byte*) malloc( rowbytes*calcH );
				for ( int y = 0; y < calcH; ++y ) {
					row_pointers[y] = ((png_bytep)raw_image) + rowbytes*y;
				}

				png_read_image(png_ptr, row_pointers);

				// Copy PNG data to the pixel data
				for ( int y = 0; y < calcH; ++y )
				{
					for ( int x = 0; x < calcW*calcBPP; ++x )
					{
						pixelData[x+(y*calcW*calcBPP)] = row_pointers[y][x];
					}
				}

				// Cleanup heap
				free(row_pointers);
				free(raw_image);
			}
			// Free reading
			png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );

			fclose(file);                    //close the file
		}

		// If the data is valid
		if ( pixelData != NULL )
		{
			// Check and set the bitdepth
			/*if ( calcBPP == 3 ) {
				if ( alphaMode == ALPHA_LOAD_MODE_DEFAULT ) {
					o_info.internalFormat = gfx::tex::kColorFormatRGB8;
				}
				else {
					o_info.internalFormat = gfx::tex::kColorFormatRGBA8;
				}
			}
			else if ( calcBPP == 4 ) {
				o_info.internalFormat = gfx::tex::kColorFormatRGBA8;
			}*/
		
			// Set the new texture size
			o_info.width = calcW;
			o_info.height = calcH;
			o_info.depth = 1;

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
					else if ( alphaMode == ALPHA_LOAD_MODE_KEYED ) {
						pData[ iTarget ].a = (
							(pData[ iTarget ].r == pTransTable[0].r) && 
							(pData[ iTarget ].g == pTransTable[0].g) && 
							(pData[ iTarget ].b == pTransTable[0].b)) ?
							0 : 255;
					}
					else {
						pData[ iTarget ].a = 255;
					}
				}
			}

			// Delete index tables
			delete [] pTransTable;
			// Delete the source data
			delete [] pixelData;
			// Return the final data
			return pData;
		}
		else
		{
			// Print error if data is invalid
			fprintf( stderr, "Could not read PNG file \"%s\"\n", n_inputfile );
			// Return no data read
			return NULL;
		}
	}
}}

#endif//CORE_ASSETS_TEXLOADER_PNG_HPP_