//===============================================================================================//
//
//		CMappedBinaryFile - Memory-mapped binary file IO
//
// The current implementation does not use mmap.
//
//===============================================================================================//
#ifndef C_MAPPED_BINARY_FILE_H_
#define C_MAPPED_BINARY_FILE_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"

#include <cstdio>

class CMappedBinaryFile
{
public:
	explicit	CMappedBinaryFile ( const char* n_filename );
				~CMappedBinaryFile ( void );

	// Returns if the file is ready to be worked on.
	bool		GetReady ( void );
				
	// Write changes to the disk
	void		SyncToDisk ( void );

	// Sets the buffer to 0, then reads in as much data as possible.
	// Does not expand the file.
	size_t		ReadBuffer ( void* n_buffer, const size_t n_buffer_size );
	// Writes the buffer to the file, overwriting data.
	// Expands file as necessary.
	size_t		WriteBuffer ( const void* n_buffer, const size_t n_buffer_size );

	// Seeks to the given location.
	// Will expand the file as necessary.
	void		SeekTo ( const long n_pos );

	// Returns a stream for manual I/O. May return NULL if manual I/O is not supported.
	FILE*		GetStream ( void );

	// Returns current size of the mapped buffer. May not be 100% correct.
	long		GetSize ( void );

	// Returns the current virtual memory cursor.
	long		GetCursor ( void );


private:
	FILE*		m_file;

	arstring<256>	m_filename;

private:
	long		TellPos ( void );
	long		TellSize( void );

};

#endif//C_MAPPED_BINARY_FILE_H_