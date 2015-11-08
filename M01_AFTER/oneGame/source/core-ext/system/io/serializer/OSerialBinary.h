

#ifndef _C_BINARY_SAVER_H_
#define _C_BINARY_SAVER_H_

#include "CBaseSerializer.h"

class CBinaryFile;

class OSerialBinary : public CBaseSerializer
{
public:
	explicit		OSerialBinary ( CBinaryFile* file, uint version=0 );
					~OSerialBinary ( void );

	Serializer&	operator&	( bool & ) override;
	Serializer&	operator&	( float & ) override;
	Serializer&	operator&	( double & ) override;
	Serializer&	operator&	( int8_t & ) override;
	Serializer&	operator&	( uint8_t & ) override;
	Serializer&	operator&	( int16_t & ) override;
	Serializer&	operator&	( uint16_t & ) override;
	Serializer&	operator&	( int32_t & ) override;
	Serializer&	operator&	( uint32_t & ) override;
	Serializer&	operator&	( int64_t & ) override;
	Serializer&	operator&	( uint64_t & ) override;
	Serializer&	operator&	( std::string & ) override;

	Serializer&	operator&	( Vector3d & ) override;
	Serializer&	operator&	( Quaternion & ) override;
	Serializer&	operator&	( Rotator & ) override;
	Serializer&	operator&	( Color & ) override;
	Serializer&	operator&	( arstring<128> & ) override;
	Serializer&	operator&	( arstring<256> & ) override;

	//Serializer&	operator&	( glMaterial & ) override;

	Serializer&	operator&	( std::vector<Color> & ) override;
	
	template <class Object>
	OSerialBinary& operator<<	( Object& );
	template <class Object>
	OSerialBinary& operator<<	( Object* );
private:
	CBinaryFile*	m_file;
};


template <class Object>
OSerialBinary& OSerialBinary::operator<<	( Object& obj )
{
	obj.serialize( *this, m_version );
	return *this;
}
template <class Object>
OSerialBinary& OSerialBinary::operator<<	( Object* obj )
{
	obj->serialize( *this, m_version );
	return *this;
}

#endif//_C_BINARY_SAVER_H_