//===============================================================================================//
//
//		class arBaseObject
//
//	Basic reference counted base class, used for keeping track of objects and memory leakage.
//
//===============================================================================================//
#ifndef _C_AR_BASE_OBJECT_H_
#define _C_AR_BASE_OBJECT_H_

#include "core/types/types.h"
#include "core/exceptions/exceptions.h"

class arBaseObject
{
private:
	// Disable arBaseObject copying:
	arBaseObject & operator= (const arBaseObject & other) { fnl_assert(0); return(*this); }
	arBaseObject ( const arBaseObject& other) { fnl_assert(0); }

public:
	explicit arBaseObject ( void )
		: referenceCount(1)
	{
		;
	}

	//===============================================================================================//
	// Memory Management : Reference counting
	//===============================================================================================//

	arBaseObject* RemoveReference ( void ) {
		if ( referenceCount > 0 )
			referenceCount--;
		else
			throw core::InvalidCallException();
		return this;
	}
	arBaseObject* AddReference ( void ) { referenceCount++; return this; }
	bool HasReference ( void ) { return (referenceCount != 0); }

private:
	//===============================================================================================//
	// Private data
	//===============================================================================================//

	//	referenceCount
	// used for reference counting
	uint16_t	referenceCount;
};

namespace core
{
	//		core::Orphan
	// Removes the 1st reference, effectively leaving the object with no reference.
	// To be used on an inline new object.
	template <class T>
	T* Orphan ( T* object )
	{
		return (T*)(object->RemoveReference());
	}
}

#endif//_C_AR_BASE_OOBJECT_H_