#ifndef CORE_SYSTEM_IO_FILE_WIN32STREAM_H_
#define CORE_SYSTEM_IO_FILE_WIN32STREAM_H_

#include "core/system/io/IFile.h"
#include "core/os.h"

#if PLATFORM_WINDOWS

#include <objidl.h>

namespace core
{
	class FileWin32StreamRead : public IFileRead
	{
	public:
		FileWin32StreamRead ( IStream* pStream )
			: m_stream(pStream)
		{}

		~FileWin32StreamRead ( void )
		{}

		virtual void		Seek ( const int64 where ) override
		{
			LARGE_INTEGER where_to;
			where_to.QuadPart = where;
			m_stream->Seek( where_to, STREAM_SEEK_SET, NULL );
		}

		virtual int64		Tell ( void ) override
		{
			return -1;
		}

		virtual int64		Read ( void* read_into, const int64 item_size, const int64 item_count ) override
		{
			ULONG bytes_read = 0;
			m_stream->Read( read_into, (ULONG)(item_size * item_count), &bytes_read );
			return (int64)(bytes_read / item_size);
		}

	private:
		IStream*		m_stream;
	};
}

#endif

#endif//CORE_SYSTEM_IO_FILE_WIN32STREAM_H_