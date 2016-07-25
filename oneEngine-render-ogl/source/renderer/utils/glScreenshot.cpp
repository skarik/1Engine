
#include "renderer/system/glMainSystem.h"
#include "glScreenshot.h"
#include "core/system/Screen.h"
#include "core/system/io/FileUtils.h"

#include <stdio.h>
#include <time.h>

#include "png/png.h"
#include "zlib/zlib.h"

void glScreenshot::SaveTimestampedToPNG ( void )
{
	arstring<256> filename;
	struct tm*	timeinfo;
	time_t		rawtime;

	time( &rawtime );
	timeinfo = localtime( &rawtime );
	// Get timestamped filename
	strftime( filename.data, 256, "SS_%Y_%m_%d_%H-%M-%S.png", timeinfo );

	// Get rid of duped names
	if ( IO::FileExists( filename ) ) {
		arstring<256> tempfilename = filename;
		int count = 0;
		while ( IO::FileExists( tempfilename ) ) {
			count += 1;
			sprintf( tempfilename.data, "%s-%d.png", filename.c_str(), count );
		}
		filename = tempfilename;
	}

	// Save file
	SaveToPNG( filename );
}

void glScreenshot::SaveToPNG( const arstring<256> & filename )
{	GL_ACCESS
	float* raw_pixelVals = GL.GetPixels( 0,0, Screen::Info.width,Screen::Info.height );
	uchar* rgb_pixelVals = new uchar [Screen::Info.width*Screen::Info.height*3];
	{
		for ( uint i = 0; i < Screen::Info.width*Screen::Info.height*3; ++i ) {
			rgb_pixelVals[i] = (uchar)(raw_pixelVals[i]*255);
		}
	}
	uchar** rgb_pixelRows = new uchar* [Screen::Info.height];
	{
		for ( uint i = 0; i < Screen::Info.height; ++i ) {
			rgb_pixelRows[i] = &(rgb_pixelVals[Screen::Info.width*(Screen::Info.height-i-1)*3]);
		}
	}

	FILE* file = fopen( filename, "wb" );

	png_structp  png_ptr;
    png_infop  info_ptr;
	jmp_buf jmpbuf;

	// Create and write info struct
	//png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, mainprog_ptr, writepng_error_handler, NULL );
	png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if (!png_ptr) {
        return;
	}
    info_ptr = png_create_info_struct( png_ptr );
    if ( !info_ptr ) {
        png_destroy_write_struct( &png_ptr, NULL );
        return;
    }

	// Need setjmp for error or program exits
	if (setjmp(jmpbuf)) { // Doesn't actually work >.>
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

	// Open file
	png_init_io( png_ptr, file );

	// Set compression
    png_set_compression_level( png_ptr, Z_BEST_COMPRESSION );

	// rofl
	//PNG_COLOR_TYPE_RGB
	//PNG_INTERLACE_NONE
	png_set_IHDR( png_ptr, info_ptr, Screen::Info.width, Screen::Info.height,
      8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );

	// Write all initial data
	png_write_info( png_ptr, info_ptr );

	// Set packing?
	png_set_packing( png_ptr );

	// Write image
	png_write_image( png_ptr, rgb_pixelRows );
	// Finish off
	png_write_end( png_ptr, NULL );


	// Clear temp data
	delete [] rgb_pixelRows;
	delete [] rgb_pixelVals;
	delete [] raw_pixelVals;

	// Close file
	fclose( file );
}