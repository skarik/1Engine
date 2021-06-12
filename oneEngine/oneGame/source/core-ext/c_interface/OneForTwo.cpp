#include "OneForTwo.h"

#if ONEFORTWO_ENABLED

#include "core-ext/system/io/Files.h"

double AR_CALL FileGetSize ( const char* filename )
{
	return (double)core::io::file::GetSize(filename);
}
double AR_CALL FileGetLastWriteTime ( const char* filename )
{
	return (double)core::io::file::GetLastWriteTime(filename);
}

#endif