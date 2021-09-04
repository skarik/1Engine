#ifndef CORE_SYSTEM_IO_INTERFACE_FILE_H_
#define CORE_SYSTEM_IO_INTERFACE_FILE_H_

#include "core/types/types.h"

namespace core
{
	class IFile
	{
	public:
		virtual void		Seek ( const int64 where ) = 0;
		virtual int64		Tell ( void ) = 0;
	};

	class IFileRead : public IFile
	{
	public:
		virtual int64		Read ( void* read_into, const int64 item_size, const int64 item_count ) = 0;
	};

	class IFileWrite : public IFile
	{
	public:
		virtual int64		Write ( void* write_from, const int64 item_size, const int64 item_count ) = 0;
		virtual void		Flush ( void ) = 0;
	};
}

#endif//CORE_SYSTEM_IO_INTERFACE_FILE_H_