
#include "OSerialBinary.h"

#include "core/system/io/CBinaryFile.h"
#include "core/math/Vector3d.h"
#include "core/math/Quaternion.h"
#include "core/math/Rotator.h"
#include "core/math/Color.h"
//#include "RangeVector.h"
//#include "glMaterial.h"

using std::string;
using std::vector;

OSerialBinary::OSerialBinary ( CBinaryFile* file, uint version )
	: m_file( file )
{
	m_version = version;
}

OSerialBinary::~OSerialBinary ( void )
{
	
}

Serializer&	OSerialBinary::operator&	( bool & val )
{
	m_file->WriteChar( val );
	return *this;
}
Serializer&	OSerialBinary::operator&	( float & num )
{
	m_file->WriteFloat( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( double & num )
{
	m_file->WriteDouble( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( int8_t & num )
{
	m_file->WriteChar( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( uint8_t & num )
{
	m_file->WriteUChar( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( int16_t & num )
{
	m_file->WriteShort( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( uint16_t & num )
{
	m_file->WriteUShort( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( int32_t & num )
{
	m_file->WriteInt32( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( uint32_t & num )
{
	m_file->WriteUInt32( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( int64_t & num )
{
	m_file->WriteInt64( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( uint64_t & num )
{
	m_file->WriteUInt64( num );
	return *this;
}
Serializer&	OSerialBinary::operator&	( string & str )
{
	m_file->WriteString( str );
	return *this;
}

Serializer&	OSerialBinary::operator&	( Vector3d & vec )
{
	m_file->WriteFloat( vec.x );
	m_file->WriteFloat( vec.y );
	m_file->WriteFloat( vec.z );
	return *this;
}
Serializer&	OSerialBinary::operator&	( Quaternion & qat )
{
	m_file->WriteFloat( qat.x );
	m_file->WriteFloat( qat.y );
	m_file->WriteFloat( qat.z );
	m_file->WriteFloat( qat.w );
	return *this;
}
Serializer&	OSerialBinary::operator&	( Rotator & rot )
{
	m_file->WriteData( (char*)rot.pData, sizeof(ftype)*9 );
	return *this;
}
Serializer&	OSerialBinary::operator&	( Color & col )
{
	m_file->WriteFloat( col.red );
	m_file->WriteFloat( col.green );
	m_file->WriteFloat( col.blue );
	m_file->WriteFloat( col.alpha );
	return *this;
}
Serializer&	OSerialBinary::operator&	( arstring<128> & str )
{
	//m_file->WriteString( str.c_str() );
	m_file->WriteData( str.data, 128 );
	return *this;
}
Serializer&	OSerialBinary::operator&	( arstring<256> & str )
{
	//m_file->WriteString( str.c_str() );
	m_file->WriteData( str.data, 256 );
	return *this;
}
/*
Serializer& OSerialBinary::operator&	( glMaterial& mat )
{
	m_file->WriteString( mat.getName() );
	return *this;
}
*/
Serializer& OSerialBinary::operator&	( vector<Color> & vec )
{
	m_file->WriteString( "colorarray" );
	m_file->WriteUInt32( (uint32_t)vec.size() );
	for ( uint i = 0; i < vec.size(); ++i ) {
		(*this) & vec[i];
	}
	return *this;
}