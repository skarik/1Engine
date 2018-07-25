/*
// Includes
#include "core/utils/string.h"
//#include "RrRenderer.h"
#include "RrTexture.h"
#include "RrTextureMaster.h"
#include "renderer/system/glMainSystem.h"
// Image libraries
#include "tga/tga_loader.h"
#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
#include "png/png.h"

using std::cout;
using std::endl;

// === File Loader ===
void RrTexture::LoadImageInfo ( void )
{
	// Note that in OpenGL, we load the textures upside down to keep DirectX compatible UV's.
	// Blame us for working with DirectDraw for the majority of our lives.

	// Detect the type of image
	string sExtension = core::utils::string::GetLower( core::utils::string::GetFileExtension( sFilename ) );
	if ( sExtension == "tga" )
	{
		loadTGA();
	}
	else if ( sExtension == "png" )
	{
		loadPNG();
		//loadDefault();
	}
	else if (( sExtension == "jpeg" )||( sExtension == "jpg" ))
	{
		loadJPG();
	}
	else
	{
		cout << "Couldn't load file of type '" << sExtension << "' !!" << endl;
		loadDefault();
	}
}

// ==DEFAULT XOR TEXTURE==
void RrTexture::loadDefault ( void )
{
	// Set the new texture size
	info.width = 64;
	info.height = 64;
	// Set the internal format to just RGB8
	info.internalFormat = RGB8;
	// Create the pixel data
	pData = new tPixel [ info.width * info.height ];
	// Create the data
	unsigned int iTarget;
	unsigned short int c;
	for ( unsigned int ix = 0; ix < info.width; ix += 1 )
	{
		for ( unsigned int iy = 0; iy < info.height; iy += 1 )
		{
			iTarget = ix+(iy*info.width);
			c = (ix^iy)*4;
			pData[iTarget].r = char(c);
			pData[iTarget].g = char(c);
			pData[iTarget].b = char(c);
			pData[iTarget].a = char(c);
		}
	}
}

// =TGA LOADING=
void RrTexture::loadTGA ( void )
{
	GL_ACCESS; // Using the glMainSystem accessor

	int calcW, calcH, calcBPP;
	unsigned char * pixelData = NULL;

	// Read in the data
	pixelData = RGB_TGA::rgb_tga( sFilename.c_str(), &calcW, &calcH, &calcBPP );

	// If the data is valid
	if ( pixelData != NULL )
	{
		int iResX = 1;
		int iResY = 1;
		// Check for maximum size limits
		if (( calcW < (signed)info.width )||( calcH < (signed)info.height ))
		{
			iResX = std::max<uint>( info.width/calcW, 1 );
			iResY = std::max<uint>( info.height/calcH, 1 );
		}
		// Do oldstyle texture loading if power of two loading mandatory 
		if ( !GL.NPOTsAvailable )
		{
			// Set the new texture size
			info.width = 1;
			while (( info.width < (unsigned)calcW )&&( info.width < (unsigned)GL.MaxTextureSize ))
				info.width *= 2;	
			info.height = 1;
			while (( info.height < (unsigned)calcH )&&( info.height < (unsigned)GL.MaxTextureSize ))
				info.height *= 2;
			// Check and set the bitdepth
			if ( calcBPP == 3 )
				info.internalFormat = RGB8;
			else if ( calcBPP == 4 )
				info.internalFormat = RGBA8;
			// Create the pixel data
			pData = new tPixel [ info.width * info.height ];
			// Go through the stored data and save it to the texture pixel data
			unsigned int iTarget;
			unsigned int iSource;
			for ( unsigned int ix = 0; ix < info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < info.height; iy += 1 )
				{
					iTarget = ix+(iy*info.width);
					iSource = ( int((ix/float(info.width))*calcW) + int((int((1-(iy/float(info.height)))*calcH)-1)*info.width) )*calcBPP;
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
		}
		else // Use the texture size if NPOT textures are available
		{
			// Set the new texture size
			info.width = calcW;
			info.height = calcH;
			// Check and set the bitdepth
			if ( calcBPP == 3 )
				info.internalFormat = RGB8;
			else if ( calcBPP == 4 )
				info.internalFormat = RGBA8;
			// Create the pixel data
			pData = new tPixel [ info.width * info.height ];
			// Go through the stored data and save it to the texture pixel data
			unsigned int iTarget;
			unsigned int iSource;
			for ( unsigned int ix = 0; ix < info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < info.height; iy += 1 )
				{
					iTarget = ix+(iy*info.width);
					iSource = (ix+((info.height-iy-1)*info.width))*calcBPP;
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
		}
		// Delete the source data
		delete [] pixelData;
	}
	else
	{
		// Throw error if data is invalid
		cout << "Couldn't load TGA '" << sFilename << "' \n";
		// Give a default texture
		loadDefault();
	}
}
// =JPG LOADING=
void RrTexture::loadJPG ( void )
{
	GL_ACCESS; // Using the glMainSystem accessor

	int calcW, calcH, calcBPP;
	unsigned char * pixelData = NULL;

	// Read in the data
	//pixelData = RGB_TGA::rgb_tga( sFilename.c_str(), &calcW, &calcH, &calcBPP );
	FILE* file = fopen( sFilename.c_str(), "rb");  //open the file
	struct jpeg_decompress_struct info_jpg;  //the jpeg decompress info
	struct jpeg_error_mgr err;           //the error handler

	info_jpg.err = jpeg_std_error(&err);     //tell the jpeg decompression handler to send the errors to err
	jpeg_create_decompress(&info_jpg);       //sets info to all the default stuff

	//if the jpeg file didnt load exit
	if ( !file )
	{
		fprintf( stderr, "Error reading JPEG file %s!!!", sFilename.c_str() );
		pixelData = NULL;
	}
	else
	{
		jpeg_stdio_src(&info_jpg, file);    //tell the jpeg lib the file we'er reading
		jpeg_read_header(&info_jpg, TRUE);   //tell it to start reading it
	

		info_jpg.do_fancy_upsampling = FALSE; //read it fast

		jpeg_start_decompress(&info_jpg);    //decompress the file

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

		jpeg_finish_decompress(&info_jpg);   //finish decompressing this file

		jpeg_destroy_decompress(&info_jpg); //free info

		fclose(file);                    //close the file
	}

	// If the data is valid
	if ( pixelData != NULL )
	{
		int iResX = 1;
		int iResY = 1;
		// Check for maximum size limits
		if (( calcW < (signed)info.width )||( calcH < (signed)info.height ))
		{
			iResX = std::max<uint>( info.width/calcW, 1 );
			iResY = std::max<uint>( info.height/calcH, 1 );
		}
		// Do oldstyle texture loading if power of two loading mandatory 
		if ( !GL.NPOTsAvailable )
		{
			// Set the new texture size
			info.width = 1;
			while (( info.width < (unsigned)calcW )&&( info.width < (unsigned)GL.MaxTextureSize ))
				info.width *= 2;	
			info.height = 1;
			while (( info.height < (unsigned)calcH )&&( info.height < (unsigned)GL.MaxTextureSize ))
				info.height *= 2;
			// Check and set the bitdepth
			if ( calcBPP == 3 )
				info.internalFormat = RGB8;
			else if ( calcBPP == 4 )
				info.internalFormat = RGBA8;
			// Create the pixel data
			pData = new tPixel [ info.width * info.height ];
			// Go through the stored data and save it to the texture pixel data
			unsigned int iTarget;
			unsigned int iSource;
			for ( unsigned int ix = 0; ix < info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < info.height; iy += 1 )
				{
					iTarget = ix+((info.height-iy-1)*info.width);
					iSource = ( int((ix/float(info.width))*calcW) + int((int((1-(iy/float(info.height)))*calcH)-1)*info.width) )*calcBPP;
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
		}
		else // Use the texture size if NPOT textures are available
		{
			// Set the new texture size
			info.width = calcW;
			info.height = calcH;
			// Check and set the bitdepth
			if ( calcBPP == 3 )
				info.internalFormat = RGB8;
			else if ( calcBPP == 4 )
				info.internalFormat = RGBA8;
			// Create the pixel data
			pData = new tPixel [ info.width * info.height ];
			// Go through the stored data and save it to the texture pixel data
			unsigned int iTarget;
			unsigned int iSource;
			for ( unsigned int ix = 0; ix < info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < info.height; iy += 1 )
				{
					iTarget = ix+((info.height-iy-1)*info.width);
					iSource = (ix+((info.height-iy-1)*info.width))*calcBPP;
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
		}
		// Delete the source data
		delete [] pixelData;
	}
	else
	{
		// Throw error if data is invalid
		//cout << "Couldn't load TGA '" << sFilename << "' \n";
		// Give a default texture
		loadDefault();
	}
}


// =PNG LOADING=
void RrTexture::loadPNG ( void )
{
	GL_ACCESS; // Using the glMainSystem accessor

	int calcW, calcH, calcBPP;
	unsigned char * pixelData = NULL;

	// Read in the data
	FILE* file = fopen( sFilename.c_str(), "rb");  //open the file

	if ( !file )
	{
		fprintf( stderr, "Error reading PNG file %s!!!", sFilename.c_str() );
		pixelData = NULL;
	}
	else
	{
		unsigned char header[8];    // 8 is the maximum size that can be checked
		fread( header, 1, 8, file );
		if ( png_sig_cmp( header, 0, 8 ) )
			fprintf( stderr, "File %s is not recognized as a PNG file", sFilename.c_str() );

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
        //color_type = png_get_color_type(png_ptr, info_ptr);
        //calcBPP = png_get_bit_depth(png_ptr, info_ptr);
		//calcBPP = png_get_bit_depth(png_ptr, info_ptr) / 2;
		int color_type, bit_depth;
		color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		if ( color_type == PNG_COLOR_TYPE_RGBA ) {
			calcBPP = (bit_depth / 8)*4;
		}
		else if ( color_type == PNG_COLOR_TYPE_RGB ) {
			calcBPP = (bit_depth / 8)*3;
		}
		else {
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
		int iResX = 1;
		int iResY = 1;
		// Check for maximum size limits
		if (( calcW < (signed)info.width )||( calcH < (signed)info.height ))
		{
			iResX = std::max<uint>( info.width/calcW, 1 );
			iResY = std::max<uint>( info.height/calcH, 1 );
		}
		// Do oldstyle texture loading if power of two loading mandatory 
		if ( !GL.NPOTsAvailable )
		{
			// Set the new texture size
			info.width = 1;
			while (( info.width < (unsigned)calcW )&&( info.width < (unsigned)GL.MaxTextureSize ))
				info.width *= 2;	
			info.height = 1;
			while (( info.height < (unsigned)calcH )&&( info.height < (unsigned)GL.MaxTextureSize ))
				info.height *= 2;
			// Check and set the bitdepth
			if ( calcBPP == 3 )
				info.internalFormat = RGB8;
			else if ( calcBPP == 4 )
				info.internalFormat = RGBA8;
			// Create the pixel data
			pData = new tPixel [ info.width * info.height ];
			// Go through the stored data and save it to the texture pixel data
			unsigned int iTarget;
			unsigned int iSource;
			for ( unsigned int ix = 0; ix < info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < info.height; iy += 1 )
				{
					iTarget = ix+((info.height-iy-1)*info.width);
					iSource = ( int((ix/float(info.width))*calcW) + int((int((1-(iy/float(info.height)))*calcH)-1)*info.width) )*calcBPP;
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
		}
		else // Use the texture size if NPOT textures are available
		{
			// Set the new texture size
			info.width = calcW;
			info.height = calcH;
			// Check and set the bitdepth
			if ( calcBPP == 3 )
				info.internalFormat = RGB8;
			else if ( calcBPP == 4 )
				info.internalFormat = RGBA8;
			// Create the pixel data
			pData = new tPixel [ info.width * info.height ];
			// Go through the stored data and save it to the texture pixel data
			unsigned int iTarget;
			unsigned int iSource;
			for ( unsigned int ix = 0; ix < info.width; ix += 1 )
			{
				for ( unsigned int iy = 0; iy < info.height; iy += 1 )
				{
					iTarget = ix+((info.height-iy-1)*info.width);
					iSource = (ix+((info.height-iy-1)*info.width))*calcBPP;
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
		}
		// Delete the source data
		delete [] pixelData;
	}
	else
	{
		// Throw error if data is invalid
		cout << "Couldn't load PNG '" << sFilename << "' \n";
		// Give a default texture
		loadDefault();
	}
}
*/