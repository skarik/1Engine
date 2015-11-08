
#ifndef _C_AR_PROPERTY_H_
#define _C_AR_PROPERTY_H_

//	class arproperty
// Wraps a read-only property
template <typename Type>
class arproperty
{
private:
	Type&	_value;

public:
	explicit		arproperty ( Type& value )
		: _value( value )
	{
		;
	}

	Type& operator-> ( void ) {
		return _value;
	}
	const Type& operator-> ( void ) const {
		return _value;
	}

	Type& operator() ( void ) {
		return _value;
	}
	const Type& operator() ( void ) const {
		return _value;
	}
};

#endif//_C_AR_PROPERTY_H_