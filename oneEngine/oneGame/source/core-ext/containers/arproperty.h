//===============================================================================================//
//
//		arproperty
//
// Class that mimics get/set of C#. Can be used as a basis for a "safe" accessor.
// Usage is not recommended.
//
//===============================================================================================//
#ifndef CORE_CONTAINER_AR_PROPERTY_H_
#define CORE_CONTAINER_AR_PROPERTY_H_

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

#endif//CORE_CONTAINER_AR_PROPERTY_H_