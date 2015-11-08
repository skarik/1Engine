// CMappedBinaryFile
// Memory-mapped binary file IO.
// The current implementation does not use mmap.

#ifndef _C_MAPPED_BINARY_FILE_H_
#define _C_MAPPED_BINARY_FILE_H_

#include <cstdio>
#include "core/types/types.h"
#include "core/containers/arstring.h"

class CMappedBinaryFile
{
public:
	explicit	CMappedBinaryFile ( const char* n_filename );
				~CMappedBinaryFile ( void );
				
	// Write changes to the disk
	void		SyncToDisk ( void );

	// Sets the buffer to 0, then reads in as much data as possible.
	// Does not expand the file.
	uint32_t	ReadBuffer ( void* n_buffer, const uint32_t n_buffer_size );
	// Writes the buffer to the file, overwriting data.
	// Expands file as necessary.
	uint32_t	WriteBuffer ( const void* n_buffer, const uint32_t n_buffer_size );

	// Seeks to the given location.
	// Will expand the file as necessary.
	void		SeekTo ( const uint32_t n_pos );

	// Returns a stream for manual I/O. May return NULL if manual I/O is not supported.
	FILE*		GetStream ( void );


private:
	FILE*		m_file;

	arstring<256>	m_filename;

private:
	uint32_t	TellPos ( void );
	uint32_t	TellSize( void );

};

#endif//_C_MAPPED_BINARY_FILE_H_