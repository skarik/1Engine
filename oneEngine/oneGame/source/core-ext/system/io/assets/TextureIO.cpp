//#include "renderer/state/CRenderState.h"
//#include "renderer/system/glMainSystem.h"
//#include "core/gfx/textureFormats.h"
#include "TextureIO.h"
// Image libraries
/*#include "tga/tga_loader.h"
#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
#include "png/png.h"
#include "zlib/zlib.h"*/

#include "core/utils/string.h"

#include "core/system/io/CBinaryFile.h"
#include "core/math/Math.h"

//#include <iostream>
//using std::cout;
//using std::endl;

//=========================================//
//		Image loader
//=========================================//
//tPixel* Textures::LoadRawImageData ( const std::string& n_inputfile, timgInfo& o_info )
//{
//	// Note that in OpenGL, we load the textures upside down to keep DirectX compatible UV's.
//	// Blame us for working with DirectDraw for the majority of our lives.
//
//	// Detect the type of image
//	std::string sExtension = core::utils::string::GetLower( core::utils::string::GetFileExtension( n_inputfile ) );
//	if ( sExtension == "tga" )
//	{
//		return loadTGA(n_inputfile, o_info);
//	}
//	else if ( sExtension == "bmp" )
//	{
//		return loadBMP(n_inputfile, o_info);
//	}
//	else if ( sExtension == "png" )
//	{
//		return loadPNG(n_inputfile, o_info);
//	}
//	else if (( sExtension == "jpeg" )||( sExtension == "jpg" ))
//	{
//		return loadJPG(n_inputfile, o_info);
//	}
//	else if ( sExtension == "bpd" )
//	{
//		return loadBPD(n_inputfile, o_info, -1);
//	}
//	else
//	{
//		fprintf( stderr, "There is no support for loading the file \"%s\"\n", n_inputfile.c_str() );
//		return loadDefault(o_info);
//	}
//}
//
////=========================================//
////		Debug Texture Creator
//// Create a debug XOR texture, using char values
////=========================================//
//tPixel* Textures::loadDefault ( timgInfo& o_info )
//{
//	// Set the new texture size
//	o_info.width = 64;
//	o_info.height = 64;
//	// Set the internal format to just RGB8
//	o_info.internalFormat = RGB8;
//	// Create the pixel data
//	tPixel* pData = new tPixel [ o_info.width * o_info.height ];
//	// Create the data
//	unsigned int iTarget;
//	unsigned short int c;
//	for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//	{
//		for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//		{
//			iTarget = ix+(iy*o_info.width);
//			c = (ix^iy)*4;
//			pData[iTarget].r = char(c);
//			pData[iTarget].g = char(c);
//			pData[iTarget].b = char(c);
//			pData[iTarget].a = char(c);
//		}
//	}
//	return pData;
//}
//
////=========================================//
////		TGA Loader
//// Most simple file format, by far.
//// Added because TGAs are very straightforward to load.
////=========================================//
//tPixel* Textures::loadTGA ( const std::string& n_inputfile, timgInfo& o_info )
//{
//	GL_ACCESS // Using the glMainSystem accessor
//	int calcW, calcH, calcBPP;
//	unsigned char * pixelData = NULL;
//	tPixel* pData = NULL;
//
//	// Read in the data
//	pixelData = RGB_TGA::rgb_tga( n_inputfile.c_str(), &calcW, &calcH, &calcBPP );
//
//	// If the data is valid
//	if ( pixelData != NULL )
//	{
//		int iResX = 1;
//		int iResY = 1;
//		// Check for maximum size limits
//		if (( calcW < (signed)o_info.width )||( calcH < (signed)o_info.height ))
//		{
//			iResX = std::max<uint>( o_info.width/calcW, 1 );
//			iResY = std::max<uint>( o_info.height/calcH, 1 );
//		}
//		// Do oldstyle texture loading if power of two loading mandatory 
//		if ( !GL.NPOTsAvailable )
//		{
//			// Set the new texture size
//			o_info.width = 1;
//			while (( o_info.width < (unsigned)calcW )&&( o_info.width < (unsigned)GL.MaxTextureSize ))
//				o_info.width *= 2;	
//			o_info.height = 1;
//			while (( o_info.height < (unsigned)calcH )&&( o_info.height < (unsigned)GL.MaxTextureSize ))
//				o_info.height *= 2;
//			// Check and set the bitdepth
//			if ( calcBPP == 3 )
//				o_info.internalFormat = RGB8;
//			else if ( calcBPP == 4 )
//				o_info.internalFormat = RGBA8;
//			// Create the pixel data
//			pData = new tPixel [ o_info.width * o_info.height ];
//			// Go through the stored data and save it to the texture pixel data
//			unsigned int iTarget;
//			unsigned int iSource;
//			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//			{
//				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//				{
//					iTarget = ix+(iy*o_info.width);
//					iSource = ( int((ix/float(o_info.width))*calcW) + int((int((1-(iy/float(o_info.height)))*calcH)-1)*o_info.width) )*calcBPP;
//					pData[ iTarget ].r = pixelData[ iSource+0 ];
//					pData[ iTarget ].g = pixelData[ iSource+1 ];
//					pData[ iTarget ].b = pixelData[ iSource+2 ];
//					if ( calcBPP == 4 ) {
//						pData[ iTarget ].a = pixelData[ iSource+3 ];
//					}
//					else {
//						pData[ iTarget ].a = 255;
//					}
//				}
//			}
//		}
//		else // Use the texture size if NPOT textures are available
//		{
//			// Set the new texture size
//			o_info.width = calcW;
//			o_info.height = calcH;
//			// Check and set the bitdepth
//			if ( calcBPP == 3 )
//				o_info.internalFormat = RGB8;
//			else if ( calcBPP == 4 )
//				o_info.internalFormat = RGBA8;
//			// Create the pixel data
//			pData = new tPixel [ o_info.width * o_info.height ];
//			// Go through the stored data and save it to the texture pixel data
//			unsigned int iTarget;
//			unsigned int iSource;
//			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//			{
//				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//				{
//					iTarget = ix+(iy*o_info.width);
//					iSource = (ix+((o_info.height-iy-1)*o_info.width))*calcBPP;
//					pData[ iTarget ].r = pixelData[ iSource+0 ];
//					pData[ iTarget ].g = pixelData[ iSource+1 ];
//					pData[ iTarget ].b = pixelData[ iSource+2 ];
//					if ( calcBPP == 4 ) {
//						pData[ iTarget ].a = pixelData[ iSource+3 ];
//					}
//					else {
//						pData[ iTarget ].a = 255;
//					}
//				}
//			}
//		}
//		// Delete the source data
//		delete [] pixelData;
//		// Return final data
//		return pData;
//	}
//	else
//	{
//		// Throw error if data is invalid
//		fprintf( stderr, "Could not read TGA file \"%s\"\n", n_inputfile.c_str() );
//		// Give a default texture
//		return loadDefault(o_info);
//	}
//}
//
////=========================================//
////		BMP Loader
////=========================================//
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//
//tPixel* Textures::loadBMP ( const std::string& n_inputfile, timgInfo& o_info )
//{
//	GL_ACCESS; // Using the glMainSystem accessor
//	int calcW, calcH, calcBPP;
//	unsigned char * pixelData = NULL;
//	tPixel* pData = NULL;
//
//	pixelData = stbi_load( n_inputfile.c_str(), &calcW, &calcH, &calcBPP, 4 );
//
//	// If the data is valid
//	if ( pixelData != NULL )
//	{
//		int iResX = 1;
//		int iResY = 1;
//		// Check for maximum size limits
//		if (( calcW < (signed)o_info.width )||( calcH < (signed)o_info.height ))
//		{
//			iResX = std::max<uint>( o_info.width/calcW, 1 );
//			iResY = std::max<uint>( o_info.height/calcH, 1 );
//		}
//		// Do oldstyle texture loading if power of two loading mandatory 
//		if ( !GL.NPOTsAvailable )
//		{
//			// Set the new texture size
//			o_info.width = 1;
//			while (( o_info.width < (unsigned)calcW )&&( o_info.width < (unsigned)GL.MaxTextureSize ))
//				o_info.width *= 2;	
//			o_info.height = 1;
//			while (( o_info.height < (unsigned)calcH )&&( o_info.height < (unsigned)GL.MaxTextureSize ))
//				o_info.height *= 2;
//			// Check and set the bitdepth
//			if ( calcBPP == 3 )
//				o_info.internalFormat = RGB8;
//			else if ( calcBPP == 4 )
//				o_info.internalFormat = RGBA8;
//			// Create the pixel data
//			pData = new tPixel [ o_info.width * o_info.height ];
//			// Go through the stored data and save it to the texture pixel data
//			unsigned int iTarget;
//			unsigned int iSource;
//			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//			{
//				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//				{
//					iTarget = ix+((o_info.height-iy-1)*o_info.width);
//					iSource = ( int((ix/float(o_info.width))*calcW) + int((int((1-(iy/float(o_info.height)))*calcH)-1)*o_info.width) )*calcBPP;
//					pData[ iTarget ].r = pixelData[ iSource+2 ];
//					pData[ iTarget ].g = pixelData[ iSource+1 ];
//					pData[ iTarget ].b = pixelData[ iSource+0 ];
//					if ( calcBPP == 4 ) {
//						pData[ iTarget ].a = pixelData[ iSource+3 ];
//					}
//					else {
//						pData[ iTarget ].a = 255;
//					}
//				}
//			}
//		}
//		else // Use the texture size if NPOT textures are available
//		{
//			// Set the new texture size
//			o_info.width = calcW;
//			o_info.height = calcH;
//			// Check and set the bitdepth
//			if ( calcBPP == 3 )
//				o_info.internalFormat = RGB8;
//			else if ( calcBPP == 4 )
//				o_info.internalFormat = RGBA8;
//			// Create the pixel data
//			pData = new tPixel [ o_info.width * o_info.height ];
//			// Go through the stored data and save it to the texture pixel data
//			unsigned int iTarget;
//			unsigned int iSource;
//			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//			{
//				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//				{
//					iTarget = ix+((o_info.height-iy-1)*o_info.width);
//					iSource = (ix+((o_info.height-iy-1)*o_info.width))*calcBPP;
//					pData[ iTarget ].r = pixelData[ iSource+2 ];
//					pData[ iTarget ].g = pixelData[ iSource+1 ];
//					pData[ iTarget ].b = pixelData[ iSource+0 ];
//					if ( calcBPP == 4 ) {
//						pData[ iTarget ].a = pixelData[ iSource+3 ];
//					}
//					else {
//						pData[ iTarget ].a = 255;
//					}
//				}
//			}
//		}
//		// Delete the source data
//		//delete [] pixelData;
//		stbi_image_free(pixelData);
//		// Return final data
//		return pData;
//	}
//	else
//	{
//		// Throw error if data is invalid
//		fprintf( stderr, "Could not read BMP file \"%s\"\n", n_inputfile.c_str() );
//		// Give a default texture
//		return loadDefault(o_info);
//	}
//}
//
////=========================================//
////		JPEG Loader
////=========================================//
//tPixel* Textures::loadJPG ( const std::string& n_inputfile, timgInfo& o_info )
//{
//	GL_ACCESS // Using the glMainSystem accessor
//	int calcW, calcH, calcBPP;
//	unsigned char * pixelData = NULL;
//	tPixel* pData = NULL;
//
//	// Read in the data
//	//pixelData = RGB_TGA::rgb_tga( sFilename.c_str(), &calcW, &calcH, &calcBPP );
//	FILE* file = fopen( n_inputfile.c_str(), "rb");  //open the file
//	struct jpeg_decompress_struct info_jpg;  //the jpeg decompress info
//	struct jpeg_error_mgr err;           //the error handler
//
//	info_jpg.err = jpeg_std_error(&err);     //tell the jpeg decompression handler to send the errors to err
//	jpeg_create_decompress(&info_jpg);       //sets info to all the default stuff
//
//	//if the jpeg file didnt load exit
//	if ( !file )
//	{
//		fprintf( stderr, "Could not open JPEG file \"%s\"\n", n_inputfile.c_str() );
//		pixelData = NULL;
//	}
//	else
//	{
//		jpeg_stdio_src(&info_jpg, file);    //tell the jpeg lib the file we'er reading
//		jpeg_read_header(&info_jpg, TRUE);   //tell it to start reading it
//	
//
//		info_jpg.do_fancy_upsampling = FALSE; //read it fast
//
//		jpeg_start_decompress(&info_jpg);    //decompress the file
//
//		//set the x and y size
//		calcW = info_jpg.output_width;
//		calcH = info_jpg.output_height;
//		calcBPP = info_jpg.num_components;
//
//		// Create input data
//		pixelData = new unsigned char [ calcW * calcH * calcBPP ];
//
//		unsigned char* p1 = pixelData;
//		unsigned char** p2 = &p1;
//		int numlines = 0;
//
//		while(info_jpg.output_scanline < info_jpg.output_height)
//		{
//			numlines = jpeg_read_scanlines(&info_jpg, p2, 1);
//			*p2 += numlines * calcBPP * info_jpg.output_width;
//		}
//
//		jpeg_finish_decompress(&info_jpg);   //finish decompressing this file
//
//		jpeg_destroy_decompress(&info_jpg); //free info
//
//		fclose(file);                    //close the file
//	}
//
//	// If the data is valid
//	if ( pixelData != NULL )
//	{
//		int iResX = 1;
//		int iResY = 1;
//		// Check for maximum size limits
//		if (( calcW < (signed)o_info.width )||( calcH < (signed)o_info.height ))
//		{
//			iResX = std::max<uint>( o_info.width/calcW, 1 );
//			iResY = std::max<uint>( o_info.height/calcH, 1 );
//		}
//		// Do oldstyle texture loading if power of two loading mandatory 
//		if ( !GL.NPOTsAvailable )
//		{
//			// Set the new texture size
//			o_info.width = 1;
//			while (( o_info.width < (unsigned)calcW )&&( o_info.width < (unsigned)GL.MaxTextureSize ))
//				o_info.width *= 2;	
//			o_info.height = 1;
//			while (( o_info.height < (unsigned)calcH )&&( o_info.height < (unsigned)GL.MaxTextureSize ))
//				o_info.height *= 2;
//			// Check and set the bitdepth
//			if ( calcBPP == 3 )
//				o_info.internalFormat = RGB8;
//			else if ( calcBPP == 4 )
//				o_info.internalFormat = RGBA8;
//			// Create the pixel data
//			pData = new tPixel [ o_info.width * o_info.height ];
//			// Go through the stored data and save it to the texture pixel data
//			unsigned int iTarget;
//			unsigned int iSource;
//			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//			{
//				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//				{
//					iTarget = ix+((o_info.height-iy-1)*o_info.width);
//					iSource = ( int((ix/float(o_info.width))*calcW) + int((int((1-(iy/float(o_info.height)))*calcH)-1)*o_info.width) )*calcBPP;
//					pData[ iTarget ].r = pixelData[ iSource+0 ];
//					pData[ iTarget ].g = pixelData[ iSource+1 ];
//					pData[ iTarget ].b = pixelData[ iSource+2 ];
//					if ( calcBPP == 4 ) {
//						pData[ iTarget ].a = pixelData[ iSource+3 ];
//					}
//					else {
//						pData[ iTarget ].a = 255;
//					}
//				}
//			}
//		}
//		else // Use the texture size if NPOT textures are available
//		{
//			// Set the new texture size
//			o_info.width = calcW;
//			o_info.height = calcH;
//			// Check and set the bitdepth
//			if ( calcBPP == 3 )
//				o_info.internalFormat = RGB8;
//			else if ( calcBPP == 4 )
//				o_info.internalFormat = RGBA8;
//			// Create the pixel data
//			pData = new tPixel [ o_info.width * o_info.height ];
//			// Go through the stored data and save it to the texture pixel data
//			unsigned int iTarget;
//			unsigned int iSource;
//			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//			{
//				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//				{
//					iTarget = ix+((o_info.height-iy-1)*o_info.width);
//					iSource = (ix+((o_info.height-iy-1)*o_info.width))*calcBPP;
//					pData[ iTarget ].r = pixelData[ iSource+0 ];
//					pData[ iTarget ].g = pixelData[ iSource+1 ];
//					pData[ iTarget ].b = pixelData[ iSource+2 ];
//					if ( calcBPP == 4 ) {
//						pData[ iTarget ].a = pixelData[ iSource+3 ];
//					}
//					else {
//						pData[ iTarget ].a = 255;
//					}
//				}
//			}
//		}
//		// Delete the source data
//		delete [] pixelData;
//		// Return the target data
//		return pData;
//	}
//	else
//	{
//		// Throw error if data is invalid
//		fprintf( stderr, "Could not read JPEG file \"%s\"\n", n_inputfile.c_str() );
//		// Give a default texture
//		return loadDefault(o_info);
//	}
//}
//
////=========================================//
////		PNG Loader
////=========================================//
//tPixel* Textures::loadPNG ( const std::string& n_inputfile, timgInfo& o_info )
//{
//	GL_ACCESS // Using the glMainSystem accessor
//
//	int calcW, calcH, calcBPP;
//	unsigned char * pixelData = NULL;
//	tPixel* pData = NULL;
//
//	alphaLoadMode_t alphaMode = ALPHA_LOAD_MODE_DEFAULT;
//	int iTransSize = 0;
//	tPixel* pTransTable = NULL;
//
//	// Read in the data
//	FILE* file = fopen( n_inputfile.c_str(), "rb");  //open the file
//
//	if ( !file )
//	{
//		fprintf( stderr, "Could not open PNG file \"%s\"\n", n_inputfile.c_str() );
//		pixelData = NULL;
//	}
//	else
//	{
//		unsigned char header[8];    // 8 is the maximum size that can be checked
//		fread( header, 1, 8, file );
//		if ( png_sig_cmp( header, 0, 8 ) )
//			fprintf( stderr, "File %s is not recognized as a PNG file", n_inputfile.c_str() );
//
//		// Define needed vars
//		png_structp	png_ptr;
//		png_infop	info_ptr;
//		png_infop	end_info;
//
//		// Init stuff
//		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//
//		if (!png_ptr) {
//			fprintf( stderr, "[loadPNG] png_create_read_struct failed" );
//		}
//
//		info_ptr = png_create_info_struct( png_ptr );
//
//		if (!info_ptr) {
//			png_destroy_read_struct( &png_ptr, NULL, NULL );
//            fprintf( stderr, "[loadPNG] png_create_info_struct failed" );
//		}
//        if (setjmp(png_jmpbuf(png_ptr))) {
//            fprintf( stderr, "[loadPNG] Error during init_io" );
//		}
//
//		end_info = png_create_info_struct( png_ptr );
//		if (!end_info) {
//			png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
//            fprintf( stderr, "[loadPNG] png_create_info_struct failed for end_info" );
//		}
//
//		// Now, begin the reading
//		png_init_io( png_ptr, file );
//        png_set_sig_bytes( png_ptr, 8 );
//
//        png_read_info( png_ptr, info_ptr );
//
//        calcW = png_get_image_width(png_ptr, info_ptr);
//        calcH = png_get_image_height(png_ptr, info_ptr);
// 
//		// Figure out the bit depth ajd transparency modes
//		int color_type, bit_depth;
//		png_bytep trans_alpha = NULL;
//		int num_trans = 0;
//		png_color_16p trans_color = NULL;
//
//		color_type = png_get_color_type(png_ptr, info_ptr);
//        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
//		if ( color_type == PNG_COLOR_TYPE_RGBA ) {
//			calcBPP = (bit_depth / 8)*4;
//		}
//		else if ( color_type == PNG_COLOR_TYPE_RGB ) {
//			calcBPP = (bit_depth / 8)*3;
//			// Check for tRNS alpha chunk
//			{
//				png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, &trans_color);
//				if (trans_alpha != NULL)
//				{
//					alphaMode = ALPHA_LOAD_MODE_INDEXED;
//				}
//				else if ( num_trans >= 1 )
//				{
//					iTransSize = 1;
//					pTransTable = new tPixel [1];
//					pTransTable->r = trans_color->red;
//					pTransTable->g = trans_color->green;
//					pTransTable->b = trans_color->blue;
//					pTransTable->a = trans_color->gray;
//					alphaMode = ALPHA_LOAD_MODE_KEYED;
//				}
//			}
//		}
//		else { // todo: color_type == PNG_COLOR_TYPE_GA
//			calcBPP = 1;
//		}
//		//cout << "PNG BPP: " << calcBPP << endl;
//
//		int number_of_passes;
//        number_of_passes = png_set_interlace_handling(png_ptr);
//        png_read_update_info(png_ptr, info_ptr);
//
//		// Read the file
//		if (setjmp(png_jmpbuf(png_ptr))) {
//			fprintf( stderr, "[loadPNG] Error during read_image" );
//		}
//
//		// Create input data
//		pixelData = new (std::nothrow) unsigned char [ calcW * calcH * calcBPP ];
//
//		if ( pixelData ) 
//		{
//			// Input data to file
//			uint32_t rowbytes;
//			void* raw_image;
//			png_bytep * row_pointers;
//			row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * calcH);
//			//for ( int y = 0; y < calcH; y++ )
//			//	row_pointers[y] = png_malloc( png_ptr, 
//			//        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
//			rowbytes = png_get_rowbytes(png_ptr,info_ptr);
//			raw_image = (png_byte*) malloc( rowbytes*calcH );
//			for ( int y = 0; y < calcH; ++y ) {
//				row_pointers[y] = ((png_bytep)raw_image) + rowbytes*y;
//			}
//
//			png_read_image(png_ptr, row_pointers);
//
//			// Copy PNG data to the pixel data
//			for ( int y = 0; y < calcH; ++y )
//			{
//				for ( int x = 0; x < calcW*calcBPP; ++x )
//				{
//					pixelData[x+(y*calcW*calcBPP)] = row_pointers[y][x];
//				}
//			}
//
//			// Cleanup heap
//			free(row_pointers);
//			free(raw_image);
//		}
//		// Free reading
//		png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
//
//		fclose(file);                    //close the file
//	}
//
//	// If the data is valid
//	if ( pixelData != NULL )
//	{
//		int iResX = 1;
//		int iResY = 1;
//		// Check for maximum size limits
//		if (( calcW < (signed)o_info.width )||( calcH < (signed)o_info.height ))
//		{
//			iResX = std::max<uint>( o_info.width/calcW, 1 );
//			iResY = std::max<uint>( o_info.height/calcH, 1 );
//		}
//		// Check and set the bitdepth
//		if ( calcBPP == 3 ) {
//			if ( alphaMode == ALPHA_LOAD_MODE_DEFAULT ) {
//				o_info.internalFormat = RGB8;
//			}
//			else {
//				o_info.internalFormat = RGBA8;
//			}
//		}
//		else if ( calcBPP == 4 ) {
//			o_info.internalFormat = RGBA8;
//		}
//		// Do oldstyle texture loading if power of two loading mandatory 
//		if ( !GL.NPOTsAvailable )
//		{
//			// Set the new texture size
//			o_info.width = 1;
//			while (( o_info.width < (unsigned)calcW )&&( o_info.width < (unsigned)GL.MaxTextureSize ))
//				o_info.width *= 2;	
//			o_info.height = 1;
//			while (( o_info.height < (unsigned)calcH )&&( o_info.height < (unsigned)GL.MaxTextureSize ))
//				o_info.height *= 2;
//			// Create the pixel data
//			pData = new tPixel [ o_info.width * o_info.height ];
//			// Go through the stored data and save it to the texture pixel data
//			unsigned int iTarget;
//			unsigned int iSource;
//			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//			{
//				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//				{
//					iTarget = ix+((o_info.height-iy-1)*o_info.width);
//					iSource = ( int((ix/float(o_info.width))*calcW) + int((int((1-(iy/float(o_info.height)))*calcH)-1)*o_info.width) )*calcBPP;
//					pData[ iTarget ].r = pixelData[ iSource+0 ];
//					pData[ iTarget ].g = pixelData[ iSource+1 ];
//					pData[ iTarget ].b = pixelData[ iSource+2 ];
//					if ( calcBPP == 4 ) {
//						pData[ iTarget ].a = pixelData[ iSource+3 ];
//					}
//					else if ( alphaMode == ALPHA_LOAD_MODE_KEYED ) {
//						pData[ iTarget ].a = (
//							(pData[ iTarget ].r == pTransTable[0].r) && 
//							(pData[ iTarget ].g == pTransTable[0].g) && 
//							(pData[ iTarget ].b == pTransTable[0].b)) ?
//							0 : 255;
//					}
//					else {
//						pData[ iTarget ].a = 255;
//					}
//				}
//			}
//		}
//		else // Use the texture size if NPOT textures are available
//		{
//			// Set the new texture size
//			o_info.width = calcW;
//			o_info.height = calcH;
//			// Create the pixel data
//			pData = new tPixel [ o_info.width * o_info.height ];
//			// Go through the stored data and save it to the texture pixel data
//			unsigned int iTarget;
//			unsigned int iSource;
//			for ( unsigned int ix = 0; ix < o_info.width; ix += 1 )
//			{
//				for ( unsigned int iy = 0; iy < o_info.height; iy += 1 )
//				{
//					iTarget = ix+((o_info.height-iy-1)*o_info.width);
//					iSource = (ix+((o_info.height-iy-1)*o_info.width))*calcBPP;
//					pData[ iTarget ].r = pixelData[ iSource+0 ];
//					pData[ iTarget ].g = pixelData[ iSource+1 ];
//					pData[ iTarget ].b = pixelData[ iSource+2 ];
//					if ( calcBPP == 4 ) {
//						pData[ iTarget ].a = pixelData[ iSource+3 ];
//					}
//					else if ( alphaMode == ALPHA_LOAD_MODE_KEYED ) {
//						pData[ iTarget ].a = (
//							(pData[ iTarget ].r == pTransTable[0].r) && 
//							(pData[ iTarget ].g == pTransTable[0].g) && 
//							(pData[ iTarget ].b == pTransTable[0].b)) ?
//							0 : 255;
//					}
//					else {
//						pData[ iTarget ].a = 255;
//					}
//				}
//			}
//		}
//		// Delete index tables
//		delete [] pTransTable;
//		// Delete the source data
//		delete [] pixelData;
//		// Return the final data
//		return pData;
//	}
//	else
//	{
//		// Throw error if data is invalid
//		fprintf( stderr, "Could not read PNG file \"%s\"\n", n_inputfile.c_str() );
//		// Give a default texture
//		return loadDefault(o_info);
//	}
//}

#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/assets/Conversion.h"
#include "core/debug/console.h"
#include "core/exceptions.h"

#include "zlib/zlib.h"

core::BpdLoader::BpdLoader()
	: m_loadMipmapMask(0), m_loadSuperlow(false), m_loadPalette(false), m_keepMipmapsLive(false), m_keepPaletteLive(false),
	mipmaps(), mipmapCount(0), palette(NULL),
	m_liveFile(NULL)
{
	;
}
core::BpdLoader::~BpdLoader()
{
	// Close any live file:
	if (m_liveFile != NULL) {
		fclose(m_liveFile);
		m_liveFile = NULL;
	}

	// Free up all allocated memory if data has not been grabbed
	if (!m_keepMipmapsLive)
	{
		for (int i = 0; i < 16; ++i) {
			if (mipmaps[i] != NULL) {
				delete[] mipmaps[i];
				mipmaps[i] = NULL;
			}
		}
	}
	if (!m_keepPaletteLive)
	{
		if (palette != NULL) {
			delete[] palette;
			palette = NULL;
		}
	}
}

bool core::BpdLoader::LoadBpd ( const char* n_resourcename )
{
	// Close any live file:
	if (m_liveFile != NULL) {
		fclose(m_liveFile);
		m_liveFile = NULL;
	}

	// Create the resource name
	arstring256 image_rezname  (n_resourcename);
	std::string image_filename = image_rezname;
	{
		arstring256 file_extension = core::utils::string::GetFileExtension(image_rezname);
		std::string raw_filename = image_rezname;
		core::utils::string::ToLower(file_extension, file_extension.length());
		if (file_extension.compare(""))
		{
			image_filename += ".bpd";
		}

		const char* const image_extensions[] = {
			".png", ".jpg", ".jpeg", ".gif", ".tga", ".bmp"
		};
		const size_t image_extensions_len = sizeof(image_extensions) / sizeof(const char* const);

		// Loop through and try to find the matching filename:
		bool raw_exists = false;
		for (size_t i = 0; i < image_extensions_len; ++i)
		{
			raw_filename = image_rezname + image_extensions[i];
			// Find the file to source data from:
			if (core::Resources::MakePathTo(raw_filename.c_str(), raw_filename))
			{
				raw_exists = true;
				break;
			}
		}

		// Convert file
		if (raw_exists)
		{
			if (core::Converter::ConvertFile(raw_filename.c_str()) == false)
			{
				debug::Console->PrintError( "BpdLoader::LoadBpd : Error occurred in core::Converter::ConvertFile call\n" );
			}
		}

		// Find the file to open...
		if (!core::Resources::MakePathTo(image_filename.c_str(), image_filename))
		{
			debug::Console->PrintError( "BpdLoader::LoadBpd : Could not find image file in the resources.\n" );
			return false;
		}
	}

	// Open the new file:
	m_liveFile = fopen(image_filename.c_str(), "rb");
	if (m_liveFile != NULL)
	{
		return loadBpdCommon();
	}

	return false;
	/*tPixel* pData = NULL;

	// Open the BPD
	FILE* t_bpdFile = fopen( n_inputfile.c_str(), "rb" );
	if ( t_bpdFile )
	{
		// Read in the header
		Textures::tbpdHeader bpdHeader;
		fread( &bpdHeader, sizeof(Textures::tbpdHeader), 1, t_bpdFile );
		// Skip the lowres data
		fseek( t_bpdFile, sizeof(tPixel)*16*16, SEEK_CUR );
		// Seek to the mipmap data that we're going to use
		int t_actualLevel = level;
		if ( level == -1 || level >= bpdHeader.levels-1 )
		{
			// If invalid level specified, skip all the mipmap data until the last one (that will be the highest resolution one)
			t_actualLevel = bpdHeader.levels-1;
		}
		fseek( t_bpdFile, sizeof(Textures::tbpdLevel)*(t_actualLevel), SEEK_CUR );
		// Read in that level data
		Textures::tbpdLevel bpdLevel;
		fread( &bpdLevel, sizeof(Textures::tbpdLevel), 1, t_bpdFile );

		// Set the output info
		int divisor = (int)std::pow( 2, bpdHeader.levels-1-t_actualLevel );
		o_info.width = bpdHeader.width / divisor;
		o_info.height = bpdHeader.height / divisor;
		o_info.internalFormat = RGBA8;

		// Copy over animation information
		o_info.framecount = bpdHeader.frames;
		o_info.xdivs = bpdHeader.xdivs;
		o_info.ydivs = bpdHeader.ydivs;

		// Create the pixel data
		pData = new tPixel [ o_info.width * o_info.height ];

		// Go to where the data is
		fseek( t_bpdFile, bpdLevel.offset, SEEK_SET );
		// Load in the pixel data
		uint32_t t_originalSize;
		fread( &t_originalSize, sizeof(uint32_t),1, t_bpdFile );
		uchar* t_sideBuffer = new uchar [t_originalSize];
		fread( t_sideBuffer, t_originalSize, 1, t_bpdFile );
		// Decompress the data
		unsigned long t_max_size = bpdHeader.width*bpdHeader.height*4;
		int z_result = uncompress( (uchar*)pData, &t_max_size, (uchar*)t_sideBuffer, t_originalSize );
		switch( z_result )
		{
		case Z_OK:
			break;
		case Z_MEM_ERROR:
			printf("CResourceManager::ForceLoadResource >> out of memory\n");
			break;
		case Z_BUF_ERROR:
			printf("CResourceManager::ForceLoadResource >> output buffer wasn't large enough!\n");
			break;
		case Z_DATA_ERROR:
			printf("CResourceManager::ForceLoadResource >> corrupted data!\n");
			break;
		}
		// Delete the side buffer
		delete [] t_sideBuffer;

		// Close the file
		fclose( t_bpdFile );

		// Return the pixel data
		return pData;
	}
	else
	{
		// Throw error if data is invalid
		fprintf( stderr, "Could not read BPD file \"%s\"\n", n_inputfile.c_str() );
		// Give a default texture
		return loadDefault(o_info);
	}*/
}

//	LoadBpd() : Continues loading.
// Continues to load the live BPD file initially opened with LoadBpd.
bool core::BpdLoader::LoadBpd ( void )
{
	if (m_liveFile != NULL)
	{
		return loadBpdCommon();
	}
	return false;
}

//	LoadBpdCommon() : loads BPD file
bool core::BpdLoader::loadBpdCommon ( void )
{
	fseek(m_liveFile, 0, SEEK_SET);

	bool read_header = false;

	//===============================
	// Read in the header

	textureFmtHeader header;
	if (!m_loadOnlySuperlow)
	{
		fread(&header, sizeof(header), 1, m_liveFile);
		read_header = true; 

		if (strcmp(header.head, kTextureFormat_Header) != 0)
		{
			throw core::CorruptedDataException();
			return false;
		}
	}

	// Based on that data, read in the other segments

	//===============================
	// Read in the superlow (doesnt need the header)

	if (m_buffer_Superlow)
	{
		fseek(m_liveFile, sizeof(textureFmtHeader), SEEK_SET);
		fread(m_buffer_Superlow, sizeof(gfx::arPixel) * kTextureFormat_SuperlowSize, 1, m_liveFile);
	}

	//===============================
	// Read in texture levels

	if (read_header)
	{
		for (int i = 0; i < header.levels; ++i)
		{
			if (m_buffer_Mipmaps[i])
			{
				textureFmtLevel levelInfo;

				// Read in the level info first
				fseek(m_liveFile, header.levelsOffset + sizeof(textureFmtLevel) * i, SEEK_SET);
				fread(&levelInfo, sizeof(levelInfo), 1, m_liveFile);
			
				// Ensure data is correct
				if (strcmp(levelInfo.head, kTextureFormat_HeadLevel) != 0)
				{
					throw core::CorruptedDataException();
					return false;
				}
				if (levelInfo.level != i)
				{
					debug::Console->PrintError( "BpdLoader::loadBpdCommon : Mismatch in mipmap ids, wanted %d got %d.\n", i, levelInfo.level );
					return false;
				}

				// Seek to the actual level data
				fseek(m_liveFile, levelInfo.offset, SEEK_SET);

				// Read in the data to a temp buffer
				uchar* t_sideBuffer = new uchar [levelInfo.size];
				fread(t_sideBuffer, levelInfo.size, 1, m_liveFile);
			
				// Decompress the data directly into target pointer:
				unsigned long t_effectiveWidth	= std::max<unsigned long>(1, header.width / math::exp2(i));
				unsigned long t_effectiveHeight	= std::max<unsigned long>(1, header.height / math::exp2(i));
				unsigned long t_mipmapByteCount	= sizeof(gfx::arPixel) * t_effectiveWidth * t_effectiveHeight;
				int z_result = uncompress( (uchar*)m_buffer_Mipmaps[i], &t_mipmapByteCount, (uchar*)t_sideBuffer, levelInfo.size );

				// Delete the side buffer
				delete [] t_sideBuffer;

				// Check decompress result
				switch( z_result )
				{
				case Z_OK:
					break;
				case Z_MEM_ERROR:
					debug::Console->PrintError("BpdLoader::loadBpdCommon : zlib : out of memory\n");
					break;
				case Z_BUF_ERROR:
					debug::Console->PrintError("BpdLoader::loadBpdCommon : zlib : output buffer wasn't large enough\n");
					break;
				case Z_DATA_ERROR:
					debug::Console->PrintError("BpdLoader::loadBpdCommon : zlib : corrupted data\n");
					break;
				}
			}
		}
	}

	//===============================
	// Read in animation info

	if (m_loadAnimation && read_header && header.animationOffset != 0)
	{
		fseek(m_liveFile, header.animationOffset, SEEK_SET);
		
		// Read in animation header
		textureFmtAnimation animation;
		fread(&animation, sizeof(animation), 1, m_liveFile);

		// Ensure data is correct
		if (strcmp(animation.head, kTextureFormat_HeadAnimation) != 0)
		{
			throw core::CorruptedDataException();
			return false;
		}

		// Read in all the frames too
		frames.resize(animation.frames);
		fread(frames.data(), sizeof(textureFmtFrame) * animation.frames, 1, m_liveFile);
	}

	//===============================
	// Read in palette info

	if (m_loadPalette && read_header && header.paletteOffset != 0)
	{
		fseek(m_liveFile, header.paletteOffset, SEEK_SET);
	}

	return true;
}



core::BpdWriter::BpdWriter()
{
	;
}
core::BpdWriter::~BpdWriter()
{
}

bool core::BpdWriter::WriteBpd ( const char* n_resourcename )
{
	if (datetime == 0)
	{
		// Get the time of the input file
		struct stat f_buf;
		//stat( t_inputName.c_str(), &f_buf );
		datetime = (uint64_t)f_buf.st_mtime;
	}

	FILE* file = NULL;

	if (file == NULL)
	{
		return false;
	}
	m_file = file;

	// Begin with writing the header
	if (!writeHeader() ||
		!writeLevelData())
	{
		fclose(file);
		return false;
	}

	// Write the extra fun times at the end:
	const char l_idString[] = "END OF THE LINE, ASSHOLE";
	fwrite(l_idString, sizeof(l_idString), 1, file);

	// Close the file
	fclose(file);

	return false;
}

bool core::BpdWriter::writeHeader ( void )
{
	textureFmtHeader l_header = {};
	l_header.head[0] = kTextureFormat_Header[0];
	l_header.head[1] = kTextureFormat_Header[1];
	l_header.head[2] = kTextureFormat_Header[2];
	l_header.head[3] = kTextureFormat_Header[3];
	l_header.version[0] = kTextureFormat_VersionMajor;
	l_header.version[1] = kTextureFormat_VersionMinor;
	l_header.datetime	= datetime;

	l_header.flags = 0;
	// Byte 0: Format
	l_header.flags		|= IMG_FORMAT_RGBA8 & 0x000000FF;
	// Byte 1: Transparency & load modes
	l_header.flags		|= (ALPHA_LOAD_MODE_DEFAULT << 8) & 0x0000FF00;
	// Byte 2: Type of texture (animated?)
	l_header.flags		|= (((frame_times == NULL && info.framecount == 0) ? 0 : 1) << 16) & 0x00FF0000;

	l_header.width		= info.width;
	l_header.height		= info.height;
	l_header.levels		= (uint16_t) std::max<int>( 1, math::log2( std::min<uint>(info.width, info.height) )-3 ); // -3 for 16, -4 for 32

	l_header.levelsOffset = sizeof(textureFmtHeader);
	l_header.postLevelsOffset = l_header.levelsOffset;
	// Add up the size of the levels:
	for (uint16_t i = 0; i < l_header.levels; ++i) {
		l_header.postLevelsOffset += calculateLevelSize(i);
	}

	// Write the header:
	fwrite(&l_header, sizeof(l_header), 1, m_file);

	return true;
}

bool core::BpdWriter::writeLevelData ( void )
{
	return true;
}

//=========================================//
//		BPD Loader
// probably loads in more than needed.
// TODO: minimal version of just the level loader
//=========================================//
//tPixel* Textures::loadBPD ( const std::string& n_inputfile, timgInfo& o_info, const int level )
//{
//	tPixel* pData = NULL;
//
//	// Open the BPD
//	FILE* t_bpdFile = fopen( n_inputfile.c_str(), "rb" );
//	if ( t_bpdFile )
//	{
//		// Read in the header
//		Textures::tbpdHeader bpdHeader;
//		fread( &bpdHeader, sizeof(Textures::tbpdHeader), 1, t_bpdFile );
//		// Skip the lowres data
//		fseek( t_bpdFile, sizeof(tPixel)*16*16, SEEK_CUR );
//		// Seek to the mipmap data that we're going to use
//		int t_actualLevel = level;
//		if ( level == -1 || level >= bpdHeader.levels-1 )
//		{
//			// If invalid level specified, skip all the mipmap data until the last one (that will be the highest resolution one)
//			t_actualLevel = bpdHeader.levels-1;
//		}
//		fseek( t_bpdFile, sizeof(Textures::tbpdLevel)*(t_actualLevel), SEEK_CUR );
//		// Read in that level data
//		Textures::tbpdLevel bpdLevel;
//		fread( &bpdLevel, sizeof(Textures::tbpdLevel), 1, t_bpdFile );
//
//		// Set the output info
//		int divisor = (int)std::pow( 2, bpdHeader.levels-1-t_actualLevel );
//		o_info.width = bpdHeader.width / divisor;
//		o_info.height = bpdHeader.height / divisor;
//		o_info.internalFormat = RGBA8;
//
//		// Copy over animation information
//		o_info.framecount = bpdHeader.frames;
//		o_info.xdivs = bpdHeader.xdivs;
//		o_info.ydivs = bpdHeader.ydivs;
//
//		// Create the pixel data
//		pData = new tPixel [ o_info.width * o_info.height ];
//
//		// Go to where the data is
//		fseek( t_bpdFile, bpdLevel.offset, SEEK_SET );
//		// Load in the pixel data
//		uint32_t t_originalSize;
//		fread( &t_originalSize, sizeof(uint32_t),1, t_bpdFile );
//		uchar* t_sideBuffer = new uchar [t_originalSize];
//		fread( t_sideBuffer, t_originalSize, 1, t_bpdFile );
//		// Decompress the data
//		unsigned long t_max_size = bpdHeader.width*bpdHeader.height*4;
//		int z_result = uncompress( (uchar*)pData, &t_max_size, (uchar*)t_sideBuffer, t_originalSize );
//		switch( z_result )
//		{
//		case Z_OK:
//			break;
//		case Z_MEM_ERROR:
//			printf("CResourceManager::ForceLoadResource >> out of memory\n");
//			break;
//		case Z_BUF_ERROR:
//			printf("CResourceManager::ForceLoadResource >> output buffer wasn't large enough!\n");
//			break;
//		case Z_DATA_ERROR:
//			printf("CResourceManager::ForceLoadResource >> corrupted data!\n");
//			break;
//		}
//		// Delete the side buffer
//		delete [] t_sideBuffer;
//
//		// Close the file
//		fclose( t_bpdFile );
//
//		// Return the pixel data
//		return pData;
//	}
//	else
//	{
//		// Throw error if data is invalid
//		fprintf( stderr, "Could not read BPD file \"%s\"\n", n_inputfile.c_str() );
//		// Give a default texture
//		return loadDefault(o_info);
//	}
//}