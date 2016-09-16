//===============================================================================================//
//	COctreeIO_Compression.cpp
//
// Human readable interface for zlib compression with error handling.
//===============================================================================================//

#include "COctreeIO.h"
#include "core/exceptions/exceptions.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"
/*#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif
#define CHUNK 131072*/

unsigned long	Terrain::COctreeIO::CompressBuffer ( char* n_buffer, unsigned long n_original_size, char* n_side_buffer )
{
	unsigned long compress_size;
	// Compress the data
	memcpy( n_side_buffer, n_buffer, n_original_size );
	int z_result = compress( (uchar*)n_buffer, &compress_size, (uchar*)n_side_buffer, n_original_size );
	// Check the result
	switch( z_result )
	{
	case Z_OK:
		//printf("***** SUCCESS! *****\n");
		break;
	case Z_MEM_ERROR:
		printf("Terrain::COctreeIO::CompressBuffer >> out of memory\n");
		throw std::out_of_range("Out of memory");
		break;
	case Z_BUF_ERROR:
		printf("Terrain::COctreeIO::CompressBuffer >> output buffer wasn't large enough!\n");
		throw std::out_of_range("Out of space");
		break;
	}
	// Return result size
	return compress_size;
}
unsigned long	Terrain::COctreeIO::ExpandBuffer ( char* n_buffer, unsigned long n_original_size, char* n_side_buffer, unsigned long n_max_size, unsigned long n_match_size )
{
	if ( n_match_size == 0 )
	{
		// Copy the compressed data over to temporary buffer
		//uchar* sourceBuffer = new uchar[n_original_size];
		//memcpy( sourceBuffer, n_buffer, n_original_size );
		memcpy( n_side_buffer, n_buffer, n_original_size );
		// Decompress the data
		int z_result = uncompress( (uchar*)n_buffer, &n_max_size, (uchar*)n_side_buffer, n_original_size );
		// Delete temporary buffer
		//delete [] sourceBuffer;
		// Check the result
		switch( z_result )
		{
		case Z_OK:
			//printf("***** SUCCESS! *****\n");
			break;
		case Z_MEM_ERROR:
			printf("Terrain::COctreeIO::ExpandBuffer >> out of memory\n");
			throw Core::OutOfMemoryException();
			break;
		case Z_BUF_ERROR:
			printf("Terrain::COctreeIO::ExpandBuffer >> output buffer wasn't large enough!\n");
			throw std::out_of_range("Out of space");
			break;
		case Z_DATA_ERROR:
			printf("Terrain::COctreeIO::ExpandBuffer >> corrupted data!\n");
			throw Core::CorruptedDataException();
			break;
		}
		// Return the result size
		return n_max_size;
	}
	throw std::invalid_argument( "Invalid value for n_match_size" );
	return 0;
}