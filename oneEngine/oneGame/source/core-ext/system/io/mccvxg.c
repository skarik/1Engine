
#include "mccvxg.h"

// Include default system IO
#include "stdio.h"
#include "stdlib.h"
// Include string
#include "string.h"
// Include ZLib
#include "zlib.h"


// IO general
mccVXG_file_infop	mccvxg_begin_io ( FILE* in_file )
{
	mccVXG_file_infop new_info = (mccVXG_file_infop)malloc( sizeof(mccVXG_file_info_t) );
	if (new_info)
	{
		new_info->file = in_file;
		new_info->buffer = 0;
	}
	return new_info;
}
int		mccvxg_end_io ( mccVXG_file_infop file_info )
{
	if ( file_info->file ) {
		fclose( file_info->file );
	}
	if ( file_info->buffer ) {
		free( file_info->buffer );
	}
	free( file_info );
	return 0;
}

// Input functions
int		mccvxg_read_info ( mccVXG_file_infop file_info )
{
	char tbuf [5] = {0,0,0,0,0};

	fseek( file_info->file, 0, SEEK_SET );

	// Read in header
	fread( tbuf, 4,1, file_info->file );
	if ( strcmp( tbuf, "$VXG" ) ) {
		return -2;	// Invalid header
	}

	// read in version number
	fread( &(file_info->vermajor), 4, 2, file_info->file );
	if ( file_info->vermajor < 0 || file_info->verminor < 0 ) {
		return -3;	// Invalid version
	}

	// read in pointers to other sectors
	fread( &(file_info->locimagesz), 4, 1, file_info->file );
	fread( &(file_info->locdata), 4, 1, file_info->file );

	// seek to other sectors
	if ( file_info->locimagesz )
	{
		// Seek to position
		fseek( file_info->file, file_info->locimagesz, SEEK_SET );
		// Check header
		fread( tbuf, 4,1, file_info->file );
		if ( strcmp( tbuf, "info" ) ) {
			return -5; // Invalid image loc
		}
		// read in image size and bit depth
		fread( &(file_info->xsize), 4,4, file_info->file );
	}
	else {
		return -4; // No size sector, return error
	}

	return 0;
	return 0;
}
int		mccvxg_get_bit_depth ( mccVXG_file_infop file_info )
{
	return file_info->bitdepth;
}
int		mccvxg_get_image_size ( mccVXG_file_infop file_info, int dimtype )
{
	if ( dimtype == MCCVXG_XSIZE ) return file_info->xsize;
	if ( dimtype == MCCVXG_YSIZE ) return file_info->ysize;
	if ( dimtype == MCCVXG_ZSIZE ) return file_info->zsize;
	return -1;
}
int		mccvxg_read_image ( mccVXG_file_infop file_info, void* outimagedata )
{
	char tbuf [5] = {0,0,0,0,0};
	unsigned long compress_size;
	unsigned long final_data_size;
	int z_result;

	// Bad file info, skip.
	if (file_info == NULL) {
		return -1;
	}
	
	if ( file_info->locdata ) {
		fseek( file_info->file, file_info->locdata, SEEK_SET );
	}
	else {
		return -1; // No data sector, cannot read data
	}

	// Read in header
	fread( tbuf, 4,1, file_info->file );
	if ( strcmp( tbuf, "mDTA" ) ) {
		return -2;	// Invalid header
	}

	// Create buffer
	final_data_size = file_info->bitdepth*file_info->xsize*file_info->ysize*file_info->zsize;
	compress_size = (long)(final_data_size*1.1);
	if ( !file_info->buffer ) {
		file_info->buffer = malloc( compress_size );
	}

	// Read in data
	fread( &(file_info->datasize), 4,1, file_info->file );
	fread( file_info->buffer, ((file_info->datasize+3)/4)*4,1, file_info->file );
	
	// Uncompress the data
	z_result = uncompress( (Bytef*)outimagedata, &final_data_size, (Bytef*)file_info->buffer, file_info->datasize );
	switch( z_result )
	{
	case Z_OK:
		break;
	case Z_MEM_ERROR:
		printf("out of memory\n");
		return -4; // bad uncompress
		break;
	case Z_BUF_ERROR:
		printf("output buffer wasn't large enough!\n");
		return -5; // bad uncompress
		break;
	}
	
	// Check for proper data
	if ( final_data_size != file_info->bitdepth*file_info->xsize*file_info->ysize*file_info->zsize ) {
		return -3; // Invalid data size
	}

	return 0;
}

// Output functions
void	mccvxg_write_header ( mccVXG_file_infop file_info )
{
	fseek( file_info->file, 0, SEEK_SET );

	// Read in header
	fwrite( "$VXG", 4,1, file_info->file );

	// read in version number
	file_info->vermajor = 0;
	file_info->verminor = 1;
	fwrite( &(file_info->vermajor), 4, 2, file_info->file );

	// read in pointers to other sectors
	file_info->locimagesz = 20;
	file_info->locdata = 40;
	fwrite( &(file_info->locimagesz), 4, 1, file_info->file );
	fwrite( &(file_info->locdata), 4, 1, file_info->file );
}
void	mccvxg_write_image_size ( mccVXG_file_infop file_info, int xsize, int ysize, int zsize )
{
	file_info->xsize = xsize;
	file_info->ysize = ysize;
	file_info->zsize = zsize;
}
void	mccvxg_write_bit_depth ( mccVXG_file_infop file_info, int bitdepth )
{
	file_info->bitdepth = bitdepth;
}
void	mccvxg_write_info ( mccVXG_file_infop file_info )
{
	int pos;

	pos = ftell( file_info->file );
	
	// Write info
	fwrite( "info", 4,1, file_info->file );
	fwrite( &(file_info->xsize), 4,4, file_info->file );
	
	// Write position of info 
	fseek( file_info->file, 3*4, SEEK_SET );
	fwrite( &pos, 4,1, file_info->file );
	fseek( file_info->file, 0, SEEK_END );
}
void	mccvxg_write_image ( mccVXG_file_infop file_info, void* inimagedata )
{
	int pos;
	unsigned long compress_size;
	unsigned long final_data_size;
	int z_result;

	// Write position of data
	pos = ftell( file_info->file );
	fseek( file_info->file, 4*4, SEEK_SET );
	fwrite( &pos, 4,1, file_info->file );
	fseek( file_info->file, 0, SEEK_END );

	// Write header
	fwrite( "mDTA", 4,1, file_info->file );

	// Create buffer
	final_data_size = file_info->bitdepth*file_info->xsize*file_info->ysize*file_info->zsize;
	compress_size = (long)(final_data_size*1.1);
	if ( !file_info->buffer ) {
		file_info->buffer = malloc( compress_size );
	}

	// Compress data
	z_result = compress( file_info->buffer, &compress_size, inimagedata, final_data_size );
	switch( z_result )
	{
	case Z_OK:
		break;
	case Z_MEM_ERROR:
		printf("out of memory\n");
		//return -4; // bad compress
		break;
	case Z_BUF_ERROR:
		printf("output buffer wasn't large enough!\n");
		//return -5; // bad compress
		break;
	}

	// Write data
	file_info->datasize = compress_size;
	fwrite( &(file_info->datasize), 4, 1, file_info->file ); // data length

	// Write the big data block, aligned to 32 bits
	fwrite( file_info->buffer, ((file_info->datasize+3)/4)*4,1, file_info->file );
}
