
#ifndef _C_BUFFER_IO_H_
#define _C_BUFFER_IO_H_

#include "core/types/types.h"
#include <cstring>
#include <exception>
#include <stdexcept>
#include <algorithm>

class CBufferIO
{
public:
	CBufferIO ( char* n_buffer, const uint32_t n_max_size=((uint32_t)(-1)) )
		: m_buffer(n_buffer), m_position(0), m_maxsize(n_max_size)
	{

	}

	FORCE_INLINE char		ReadChar ( void ) {
		char value = m_buffer[m_position];
		m_position++;
		return value;
	}
	FORCE_INLINE uint32_t	ReadUInt32 ( void ) {
		uint32_t value;
		memcpy( &value, &(m_buffer[m_position]), sizeof(uint32_t) );
		m_position += sizeof(uint32_t);
		return value;
	}
	FORCE_INLINE uint64_t	ReadUInt64 ( void ) {
		uint64_t value;
		memcpy( &value, &(m_buffer[m_position]), sizeof(uint64_t) );
		m_position += sizeof(uint64_t);
		return value;
	}
	FORCE_INLINE size_t		ReadData ( char* _odata, size_t _datasize )
	{
		size_t readsize = std::min<size_t>( m_maxsize-m_position, _datasize );
		memcpy( _odata, &(m_buffer[m_position]), _datasize );
		m_position += readsize;
		return readsize;
	}

	FORCE_INLINE void		WriteChar ( char _val ) {
		m_buffer[m_position] = _val;
		m_position++;
	}
	FORCE_INLINE void		WriteUInt32 ( uint32_t _val ) {
		memcpy( &(m_buffer[m_position]), &_val, sizeof(uint32_t) );
		m_position += sizeof(uint32_t);
	}
	FORCE_INLINE void		WriteUInt64 ( uint64_t& _val ) {
		memcpy( &(m_buffer[m_position]), &_val, sizeof(uint64_t) );
		m_position += sizeof(uint64_t);
	}
	FORCE_INLINE void		WriteData ( const char* _data, size_t _datasize )
	{
		memcpy( &(m_buffer[m_position]), _data, _datasize );
		m_position += _datasize;
	}


	FORCE_INLINE uint32_t	TellPosition ( void ) {
		return m_position;
	}
	FORCE_INLINE void		CheckPosition ( void ) {
		if ( m_position > m_maxsize ) {
			throw std::out_of_range( "Buffer hit ending." );
		}
	}

private:
	char*		m_buffer;
	uint32_t	m_position;
	uint32_t	m_maxsize;
};

#endif//_C_BUFFER_IO_H_
