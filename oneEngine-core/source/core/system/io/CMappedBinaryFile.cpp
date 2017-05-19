
#include "CMappedBinaryFile.h"
#include "FileUtils.h"

// TODO: In Unix, replace all ftell and fseek with POSIX ftello and fseeko.
// Reason is that for SEEK_END under Windows, fseek works as one would think, without any caveats.
// However, under other operating systems, I'm not sure of the behavior of fseek like I'm sure of the behavior of fseeko.

CMappedBinaryFile::CMappedBinaryFile ( const char* n_filename )
{
	m_filename = n_filename;

	// open file for read/write
	if ( !IO::FileExists(m_filename) ) {
		m_file = fopen( m_filename, "wb" );
		if ( m_file ) {
			fclose( m_file );
		}
	}
	m_file = fopen( m_filename, "r+b" );
}
CMappedBinaryFile::~CMappedBinaryFile ( void )
{
	// close file
	if ( m_file ) {
		fclose( m_file );
	}
}

void CMappedBinaryFile::SyncToDisk ( void )
{
	// Get cursor
	size_t prevPos = TellPos();
	// Close and open file
	fclose( m_file );
	m_file = fopen( m_filename, "r+b" );
	// Seek to position
	fseek( m_file, prevPos, SEEK_SET );
}

// Seeks to the given location.
// Will expand the file as necessary.
void CMappedBinaryFile::SeekTo ( const size_t n_pos )
{
	size_t currentSize = TellSize();
	if ( n_pos > currentSize ) {
		fseek( m_file, 0, SEEK_END );
		for ( size_t i = 0; i < n_pos-currentSize; ++i ) {
			//fwrite( "\0", 1, 1, m_file );
			fputc( 0,m_file );
		}
	}
	fseek( m_file, n_pos, SEEK_SET );
}

// Sets the buffer to 0, then reads in as much data as possible.
// Does not expand the file.
size_t CMappedBinaryFile::ReadBuffer ( void* n_buffer, const size_t n_buffer_size )
{
	memset( n_buffer, 0, n_buffer_size );
	return fread( n_buffer, 1, n_buffer_size, m_file );
}
// Writes the buffer to the file, overwriting data.
// Expands file as necessary.
size_t CMappedBinaryFile::WriteBuffer ( const void* n_buffer, const size_t n_buffer_size )
{
	return fwrite( n_buffer, 1, n_buffer_size, m_file );
}

// Returns a stream for manual I/O. May return NULL if manual I/O is not supported.
FILE* CMappedBinaryFile::GetStream ( void )
{
	return m_file;
}

size_t CMappedBinaryFile::TellPos ( void )
{
	return ftell( m_file );
}
size_t CMappedBinaryFile::TellSize( void )
{
	size_t prevPos = TellPos();
	fseek( m_file, 0, SEEK_END );
	size_t result = ftell(m_file);
	fseek( m_file, prevPos, SEEK_SET );
	return result;
}