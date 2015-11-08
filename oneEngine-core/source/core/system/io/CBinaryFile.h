// CBinaryFile
// Wrapper for C file IO functions that takes into account threads.
// Has functions for locks and stores a thread-safe list of open files.

#ifndef _C_BINARY_FILE_
#define _C_BINARY_FILE_

#include <cstdio>
#include <string>
using std::string;
#include "core/types/types.h"

#include <vector>
//using std::vector;
#include <algorithm>
//using std::find;

#include <sys/stat.h>

#include "core/math/Vector2d.h"
#include "core/math/Vector3d.h"
#include "core/math/Color.h"

//#include "boost/thread.hpp"
#include <thread>
#include <mutex>


class CBinaryFile
{
public:
	// Prepares a CBinaryFile to open a file for locked read/write.
	CBinaryFile ( void );
	// Prepares a CBinaryFile with an already opened stream. There are no locks.
	CBinaryFile ( FILE* );
	~CBinaryFile ( void );

	// Constants
	static const int IO_READ	= 1;
	static const int IO_WRITE	= 2;
	static const int IO_APPEND	= 4;

	bool	Exists ( const char * );
	bool	Open ( const char *, int );
	bool	OpenWait ( const char *, int );
	void	Close ( void );

	// Thread Locks
	FORCE_INLINE void	Lock ( void );
	FORCE_INLINE void	Unlock ( void );

	// IO State
	bool	IsOpen ( void );
	FORCE_INLINE FILE*	GetFILE ( void ) { return pFile; };
	long	GetFilesize ( void );

	// IO Functions
	void	WriteChar ( char );
	void	WriteUChar ( unsigned char );
	void	WriteShort ( short );
	void	WriteUShort ( unsigned short );
	void	WriteLong ( long );
	void	WriteULong ( unsigned long );
	void	WriteInt32 ( int32_t );
	void	WriteUInt32 ( uint32_t );
	void	WriteInt64 ( int64_t );
	void	WriteUInt64 ( uint64_t );
	void	WriteFloat ( float );
	void	WriteDouble ( double );
	void	WriteString ( const string & );
	void	WriteVector2d ( const Vector2d & );
	void	WriteVector3d ( const Vector3d & );
	void	WriteColor ( const Color & );

	void	WriteData ( const char* data, unsigned int datasize );

	char	ReadChar ( void );
	unsigned char	ReadUChar ( void );
	short	ReadShort ( void );
	unsigned short	ReadUShort ( void );
	long	ReadLong ( void );
	unsigned long	ReadULong ( void );
	int32_t		ReadInt32 ( void );
	uint32_t	ReadUInt32 ( void );
	int64_t		ReadInt64 ( void );
	uint64_t	ReadUInt64 ( void );
	float	ReadFloat ( void );
	double	ReadDouble ( void );
	string	ReadString ( int=0 );
	Vector2d	ReadVector2d ( void );
	Vector3d	ReadVector3d ( void );
	Color	ReadColor ( void );

	size_t	ReadUCharArray ( unsigned char *, unsigned int );

	size_t	ReadData ( char* data, unsigned int datasize );

protected:
	int		iOpenmode;
	bool	bIsOpen;
	FILE*	pFile;
	string	sCurrentFileName;
	bool	bLocked;
	bool	bIsStream;

	bool	bHitEoF;
	FORCE_INLINE	bool	IsEOF ( void ) {
		if ( !bHitEoF ) {
			bHitEoF = (feof(pFile)!=0);
		}
		return bHitEoF;
	}

	static std::vector<string>		v_sOpenFiles;
	static std::mutex				m_listGuard;
};

#endif