#ifndef CORE_SYSTEM_IO_FILE_STDIO_H_
#define CORE_SYSTEM_IO_FILE_STDIO_H_

#include "core/system/io/IFile.h"
#include <cstdio>

namespace core
{
	class FileStdioRead : public IFileRead
	{
	public:
		FileStdioRead ( FILE* file )
			: m_file(file)
		{}

		~FileStdioRead ( void )
		{}

		virtual void		Seek ( const int64 where ) override
		{
			//fseeko64( m_file, 
			fseek( m_file, (long)where, SEEK_SET );
		}

		virtual int64		Tell ( void ) override
		{
			return (int64)ftell( m_file );
		}

		virtual int64		Read ( void* read_into, const int64 item_size, const int64 item_count ) override
		{
			return (int64)fread( read_into, (size_t)item_size, (size_t)item_count, m_file );
		}

	private:
		FILE*		m_file;
	};
}

#endif//CORE_SYSTEM_IO_FILE_STDIO_H_