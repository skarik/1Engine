
#include "ISerialBinary.h"

#include "core/system/io/CBinaryFile.h"
#include "core/math/Vector3d.h"
#include "core/math/Quaternion.h"
#include "core/math/Rotator.h"
#include "core/math/Color.h"
//#include "RangeVector.h"
//#include "glMaterial.h"

#include "core/exceptions.h"

using std::string;
using std::vector;

ISerialBinary::ISerialBinary ( CBinaryFile* file, uint version )
	: m_file( file )
{
	m_version = version;
}

ISerialBinary::~ISerialBinary ( void )
{
	
}

Serializer&	ISerialBinary::operator&	( bool & val )
{
	val = (m_file->ReadChar()!=0);
	return *this;
}
Serializer&	ISerialBinary::operator&	( float & num )
{
	num = m_file->ReadFloat();
	return *this;
}
Serializer&	ISerialBinary::operator&	( double & num )
{
	num = m_file->ReadDouble();
	return *this;
}
Serializer&	ISerialBinary::operator&	( int8_t & num )
{
	num = m_file->ReadChar();
	return *this;
}
Serializer&	ISerialBinary::operator&	( uint8_t & num )
{
	num = m_file->ReadUChar();
	return *this;
}
Serializer&	ISerialBinary::operator&	( int16_t & num )
{
	num = m_file->ReadShort();
	return *this;
}
Serializer&	ISerialBinary::operator&	( uint16_t & num )
{
	num = m_file->ReadUShort();
	return *this;
}
Serializer&	ISerialBinary::operator&	( int32_t & num )
{
	num = m_file->ReadInt32();
	return *this;
}
Serializer&	ISerialBinary::operator&	( uint32_t & num )
{
	num = m_file->ReadUInt32();
	return *this;
}
Serializer&	ISerialBinary::operator&	( int64_t & num )
{
	num = m_file->ReadInt64();
	return *this;
}
Serializer&	ISerialBinary::operator&	( uint64_t & num )
{
	num = m_file->ReadUInt64();
	return *this;
}
Serializer&	ISerialBinary::operator&	( string & str )
{
	str = m_file->ReadString();
	return *this;
}

Serializer&	ISerialBinary::operator&	( Vector3d & vec )
{
	vec.x = m_file->ReadFloat();
	vec.y = m_file->ReadFloat();
	vec.z = m_file->ReadFloat();
	return *this;
}
Serializer&	ISerialBinary::operator&	( Quaternion & qat )
{
	qat.x = m_file->ReadFloat();
	qat.y = m_file->ReadFloat();
	qat.z = m_file->ReadFloat();
	qat.w = m_file->ReadFloat();
	return *this;
}
Serializer&	ISerialBinary::operator&	( Rotator & rot )
{
	m_file->ReadData( (char*)rot.pData, sizeof( ftype ) * 9 );
	return *this;
}
Serializer& ISerialBinary::operator&	( Color & col )
{
	col.red		= m_file->ReadFloat();
	col.green	= m_file->ReadFloat();
	col.blue	= m_file->ReadFloat();
	col.alpha	= m_file->ReadFloat();
	return *this;
}
Serializer&	ISerialBinary::operator&	( arstring<128> & str )
{
	m_file->ReadData( str.data, 128 );
	//str = m_file->ReadString().c_str();
	return *this;
}
Serializer&	ISerialBinary::operator&	( arstring<256> & str )
{
	m_file->ReadData( str.data, 256 );
	//str = m_file->ReadString().c_str();
	return *this;
}
/*
Serializer& ISerialBinary::operator&	( glMaterial& mat )
{
	string matname = m_file->ReadString();
	mat.loadFromFile( matname.c_str() );
	return *this;
}
*/
Serializer& ISerialBinary::operator&	( vector<Color>& vec )
{
	string check = m_file->ReadString();
	if ( check != "colorarray" )
	{
		throw Core::CorruptedDataException();
		std::cout << "BAD INPUT: " << check << std::endl;
	}
	uint32_t count = m_file->ReadUInt32();
	vec.clear();
	for ( uint32_t i = 0; i < count; ++i )
	{
		Color color;
		(*this) & color;
		vec.push_back( color );
	}
	return *this;
}