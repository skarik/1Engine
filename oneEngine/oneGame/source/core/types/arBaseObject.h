//===============================================================================================//
//
//		class arBaseObject
//
//	Basic reference counted base class, used for keeping track of objects and memory leakage.
//
//===============================================================================================//
#ifndef AR_BASE_OBJECT_H_
#define AR_BASE_OBJECT_H_

#include "core/types/types.h"
#include "core/exceptions/exceptions.h"
#include <atomic>

class arBaseObject
{
private:
	// Disable arBaseObject copying:
	arBaseObject& operator= ( const arBaseObject & other ) = delete;
	arBaseObject ( const arBaseObject& other ) = delete;
	arBaseObject ( const arBaseObject&& other ) = delete;

public:
	explicit arBaseObject ( void )
		: referenceCount(1)
	{
		;
	}

	//===============================================================================================//
	// Memory Management : Reference counting
	//===============================================================================================//

	arBaseObject* RemoveReference ( void )
	{
		if ( referenceCount.fetch_sub(1) == 0 )
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
	std::atomic_uint16_t	referenceCount;
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

#endif//AR_BASE_OBJECT_H_