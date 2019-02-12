#ifndef C_BASE_SERIALIZER_H_
#define C_BASE_SERIALIZER_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "core/math/Vector3.h"
#include <string>
#include <vector>

class Quaternion;
class Rotator;
class Color;
//class RrMaterial;
template <typename type>
class RangeValue;

class CBaseSerializer
{
public:
	virtual CBaseSerializer&	operator&	( bool & ) { return *this; };
	virtual CBaseSerializer&	operator&	( float & ) { return *this; };
	virtual CBaseSerializer&	operator&	( double & ) { return *this; };
	virtual CBaseSerializer&	operator&	( int8_t & ) { return *this; };
	virtual CBaseSerializer&	operator&	( uint8_t & ) { return *this; };
	virtual CBaseSerializer&	operator&	( int16_t & ) { return *this; };
	virtual CBaseSerializer&	operator&	( uint16_t & ) { return *this; };
	virtual CBaseSerializer&	operator&	( int32_t & ) { return *this; };
	virtual CBaseSerializer&	operator&	( uint32_t & ) { return *this; };
	virtual CBaseSerializer&	operator&	( int64_t & ) { return *this; };
	virtual CBaseSerializer&	operator&	( uint64_t & ) { return *this; };
	virtual CBaseSerializer&	operator&	( std::string & ) { return *this; };

	virtual CBaseSerializer&	operator&	( Vector3f & ) { return *this; };
	virtual CBaseSerializer&	operator&	( Quaternion & ) { return *this; };
	virtual CBaseSerializer&	operator&	( Rotator & ) { return *this; };
	virtual CBaseSerializer&	operator&	( Color & ) { return *this; };
	virtual CBaseSerializer&	operator&	( arstring<128> & ) { return *this; };
	virtual CBaseSerializer&	operator&	( arstring<256> & ) { return *this; };

	//virtual CBaseSerializer&	operator&	( RrMaterial & ) { return *this; };

	virtual CBaseSerializer&	operator&	( std::vector<Color> & ) { return *this; };

	// Template functions MUST be declared in the base class
	template <typename type>
	CBaseSerializer&	operator&	( RangeValue<type>& rv ) {
		(*this) & rv.mMinVal;
		(*this) & rv.mMaxVal;
		(*this) & rv.mCurVal;
		return (*this);
	}

protected:
	uint m_version;
};

typedef CBaseSerializer Serializer;

#endif//C_BASE_SERIALIZER_H_