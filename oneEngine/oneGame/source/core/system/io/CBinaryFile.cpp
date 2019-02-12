
#include "CBinaryFile.h"
#include "FileUtils.h"

std::vector<std::string>	CBinaryFile::v_sOpenFiles (16); //TODO: reserve space
std::mutex					CBinaryFile::m_listGuard;

CBinaryFile::CBinaryFile ( void )
{
	pFile = NULL;
	bIsOpen = false;
	bLocked = false;
	bHitEoF = false;
	bIsStream = false;
}

CBinaryFile::CBinaryFile ( FILE* stream )
{
	pFile = stream;
	bIsOpen = true;
	bLocked = false;
	bHitEoF = false;
	bIsStream = true;
}

CBinaryFile::~CBinaryFile ( void )
{
	bLocked = false;
	if ( bIsOpen && pFile && !bIsStream )
		Close();
}

FORCE_INLINE void	CBinaryFile::Lock ( void )
{
	while ( bLocked ) {
		;
	}
	bLocked = true;
}
FORCE_INLINE void	CBinaryFile::Unlock ( void )
{
	bLocked = false;
}

bool	CBinaryFile::Exists ( const char * sFilename )
{
	std::lock_guard<std::mutex> lock( m_listGuard );

	std::string tempFileName (sFilename);
	// Check if file is already open
	if ( find( v_sOpenFiles.begin(), v_sOpenFiles.end(), tempFileName ) != v_sOpenFiles.end() )
	{
		return true;
	}

	FILE* fp;
	fp = fopen( sFilename, "r" );
	if ( fp != NULL )
	{
		sCurrentFileName = tempFileName;
		fclose( fp );
		return true;
	}
	else
	{
		return false;
	}
}
/*bool	CBinaryFile::Exists ( string sFilename )
{
	return Exists( sFilename.c_str() );
}*/

bool	CBinaryFile::Open ( const char * sFilename, int inOpenmode )
{
	if ( bIsOpen ) {
		throw std::exception( "Trying to open an already-open file." );
	}

	std::lock_guard<std::mutex> lock( m_listGuard );

	if ( sFilename != NULL ) {
		sCurrentFileName = sFilename;
	}

	bHitEoF = false;
	bIsStream = false;

	// Check if file is already open
	if ( find( v_sOpenFiles.begin(), v_sOpenFiles.end(), sCurrentFileName ) != v_sOpenFiles.end() )
	{
		bIsOpen = false;
		return false;
	}

	iOpenmode = inOpenmode;
	if ( iOpenmode&IO_APPEND ) {
		pFile = fopen( sCurrentFileName.c_str(), "a+b" );
	}
	else if ( (iOpenmode&IO_READ) && (iOpenmode&IO_WRITE) ) {
		if ( IO::FileExists(sCurrentFileName) ) {
			pFile = fopen( sCurrentFileName.c_str(), "r+b" );
		}
		else {
			pFile = fopen( sCurrentFileName.c_str(), "wb" );
			fclose( pFile );
			pFile = fopen( sCurrentFileName.c_str(), "r+b" );
		}
	}
	else if ( iOpenmode&IO_READ ) {
		pFile = fopen( sCurrentFileName.c_str(), "rb" );
	}
	else if ( iOpenmode&IO_WRITE ) {
		pFile = fopen( sCurrentFileName.c_str(), "wb" );
	}
	bIsOpen = (pFile != NULL);

	// Add to open file list
	if ( bIsOpen )
	{
		v_sOpenFiles.push_back( sCurrentFileName );
	}

	return bIsOpen;
}
bool	CBinaryFile::OpenWait ( const char * sFilename, int inOpenmode )
{
	if ( bIsOpen ) {
		throw std::exception( "Trying to open an already-open file." );
	}
	if ( sFilename != NULL ) {
		sCurrentFileName = sFilename;
	}

	// Check if file is already open, but keep checking until it's not there.
	int waitTime = 0;
	while ( !bIsOpen )
	{
		waitTime += 1;	// Give open time to loop
		std::lock_guard<std::mutex> lock( m_listGuard );
		if ( find( v_sOpenFiles.begin(), v_sOpenFiles.end(), sCurrentFileName ) != v_sOpenFiles.end() ) {
			bIsOpen = false;
		}
		else {
			bIsOpen = true;
		}
	}

	bIsOpen = false;
	
	// Now, open file, and lock list immediately to prevent race conditions.
	{
		std::lock_guard<std::mutex> lock( m_listGuard );

		iOpenmode = inOpenmode;
		if ( iOpenmode&IO_APPEND ) {
			pFile = fopen( sCurrentFileName.c_str(), "a+b" );
		}
		else if ( (iOpenmode&IO_READ) && (iOpenmode&IO_WRITE) ) {
			if ( IO::FileExists(sCurrentFileName) ) {
				pFile = fopen( sCurrentFileName.c_str(), "r+b" );
			}
			else {
				pFile = fopen( sCurrentFileName.c_str(), "wb" );
				fclose( pFile );
				pFile = fopen( sCurrentFileName.c_str(), "r+b" );
			}
		}
		else if ( iOpenmode&IO_READ ) {
			pFile = fopen( sCurrentFileName.c_str(), "rb" );
		}
		else if ( iOpenmode&IO_WRITE ) {
			pFile = fopen( sCurrentFileName.c_str(), "wb" );
		}
		bIsOpen = (pFile != NULL);

		// Add to open file list
		if ( bIsOpen ) {
			v_sOpenFiles.push_back( sCurrentFileName );
		}
	}

	return bIsOpen;
}

/*bool	CBinaryFile::Open ( string sFilename, int inOpenmode )
{
	return OpenA( sFilename.c_str(), inOpenmode );
}*/
bool	CBinaryFile::IsOpen ( void )
{
	return ((bIsOpen)&&(pFile != NULL));
}

void	CBinaryFile::Close ( void )
{
	if ( bIsStream ) {
		throw std::exception( "Can not close stream!" );
	}

	if ( bIsOpen || pFile )
	{
		std::lock_guard<std::mutex> lock( m_listGuard );

		// Check if file is already open
		std::vector<std::string>::iterator it = std::find( v_sOpenFiles.begin(), v_sOpenFiles.end(), sCurrentFileName );
		if ( it != v_sOpenFiles.end() )
			v_sOpenFiles.erase( it );
		else
			std::cout << "WARNING: (" << __LINE__ << ") closing unopened file?!" << std::endl;

		//std::cout << "Closing file \"" << sCurrentFileName << "\" " << std::endl;

		if ( bIsOpen )
			fclose( pFile );
		pFile = NULL;
		bIsOpen = false;
	}
}

long	CBinaryFile::GetFilesize ( void )
{
	if ( bIsStream ) {
		throw std::exception( "Can not stat stream!" );
	}
	struct _stat64 filestatus;
	_stat64( sCurrentFileName.c_str(), &filestatus );
	return (long)filestatus.st_size;
}

void	CBinaryFile::WriteUChar ( unsigned char c )
{
	fwrite( &c, sizeof( unsigned char ), 1, pFile );
}
void	CBinaryFile::WriteUShort ( unsigned short i )
{
	fwrite( &i, sizeof( unsigned short ), 1, pFile );
}
void	CBinaryFile::WriteChar ( char c )
{
	fwrite( &c, sizeof( char ), 1, pFile );
}
void	CBinaryFile::WriteShort ( short i )
{
	fwrite( &i, sizeof( short ), 1, pFile );
}
void	CBinaryFile::WriteInt32 ( int32_t i )
{
	fwrite( &i, sizeof( int32_t ), 1, pFile );
}
void	CBinaryFile::WriteUInt32 ( uint32_t i )
{
	fwrite( &i, sizeof( uint32_t ), 1, pFile );
}
void	CBinaryFile::WriteInt64 ( int64_t i )
{
	fwrite( &i, sizeof( int64_t ), 1, pFile );
}
void	CBinaryFile::WriteUInt64 ( uint64_t i )
{
	fwrite( &i, sizeof( uint64_t ), 1, pFile );
}
void	CBinaryFile::WriteFloat ( float f )
{
	fwrite( &f, sizeof( float ), 1, pFile );
}
void	CBinaryFile::WriteDouble ( double f )
{
	fwrite( &f, sizeof( double ), 1, pFile );
}

void	CBinaryFile::WriteString ( const std::string & str )
{
	for ( unsigned int i = 0; i < str.size(); ++i )
		WriteChar( str[i] );
	WriteChar( 0 );
}

void	CBinaryFile::WriteVector3f ( const Vector3f & vect )
{
	WriteFloat( vect.x );
	WriteFloat( vect.y );
	WriteFloat( vect.z );
}
void	CBinaryFile::WriteColor ( const Color & color )
{
	WriteFloat( color.red );
	WriteFloat( color.green );
	WriteFloat( color.blue );
	WriteFloat( color.alpha );
}

void	CBinaryFile::WriteData ( const char* data, unsigned int datasize )
{
	fwrite( data, datasize, 1, pFile );
}

#include <iostream>
using std::cout;
using std::endl;

unsigned char	CBinaryFile::ReadUChar ( void )
{
	if ( !IsEOF() ) {
		unsigned char c;
		fread( &c , sizeof( unsigned char ), 1, pFile );
		return c;
	}
	else {
		return 0;
	}
}
unsigned short	CBinaryFile::ReadUShort ( void )
{
	if ( !IsEOF() ) {
		unsigned short i;
		fread( &i , sizeof( unsigned short ), 1, pFile );
		return i;
	}
	else {
		return 0;
	}
}
char	CBinaryFile::ReadChar ( void )
{
	if ( !IsEOF() ) {
		char c;
		fread( &c , sizeof( char ), 1, pFile );
		return c;
	}
	else {
		return 0;
	}
}
short	CBinaryFile::ReadShort ( void )
{
	if ( !IsEOF() ) {
		short i;
		fread( &i , sizeof( short ), 1, pFile );
		return i;
	}
	else {
		return 0;
	}
}
int32_t	CBinaryFile::ReadInt32 ( void )
{
	if ( !IsEOF() ) {
		int32_t i;
		fread( &i , sizeof( int32_t ), 1, pFile );
		return i;
	}
	else {
		return 0;
	}
}
uint32_t	CBinaryFile::ReadUInt32 ( void )
{
	if ( !IsEOF() ) {
		uint32_t i;
		fread( &i , sizeof( uint32_t ), 1, pFile );
		return i;
	}
	else {
		return 0;
	}
}
int64_t	CBinaryFile::ReadInt64 ( void )
{
	if ( !IsEOF() ) {
		int64_t i;
		fread( &i , sizeof( int64_t ), 1, pFile );
		return i;
	}
	else {
		return 0;
	}
}
uint64_t	CBinaryFile::ReadUInt64 ( void )
{
	if ( !IsEOF() ) {
		uint64_t i;
		fread( &i , sizeof( uint64_t ), 1, pFile );
		return i;
	}
	else {
		return 0;
	}
}
float	CBinaryFile::ReadFloat ( void )
{
	if ( !IsEOF() ) {
		float f;
		fread( &f , sizeof( float ), 1, pFile );
		return f;
	}
	else {
		return 0;
	}
}
double	CBinaryFile::ReadDouble ( void )
{
	if ( !IsEOF() ) {
		double f;
		fread( &f , sizeof( double ), 1, pFile );
		return f;
	}
	else {
		return 0;
	}
}

std::string	CBinaryFile::ReadString ( int i )
{
	std::string result = "";
	char c = ReadChar();
	if ( c != 0 )
	{
		do
		{
			result += c;
			c = ReadChar();	
		}
		while ( c != 0 );
	}
	return result;
}

Vector3f	CBinaryFile::ReadVector3f ( void )
{
	Vector3f vect;
	vect.x = ReadFloat();
	vect.y = ReadFloat();
	vect.z = ReadFloat();
	return vect;
}
Color	CBinaryFile::ReadColor ( void )
{
	Color color;
	color.red = ReadFloat();
	color.green = ReadFloat();
	color.blue = ReadFloat();
	color.alpha = ReadFloat();
	return color;
}

size_t	CBinaryFile::ReadUCharArray ( unsigned char * pA, unsigned int size )
{
	if ( !IsEOF() ) {
		return fread( pA , 1, size, pFile );
	}
	else {
		return 0;
	}
}

size_t	CBinaryFile::ReadData ( char * data, unsigned int datasize )
{
	if ( !IsEOF() ) {
		return fread( data, datasize, 1, pFile );
	}
	else {
		return 0;
	}
}