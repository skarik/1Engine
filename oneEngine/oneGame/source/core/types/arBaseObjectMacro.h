//===============================================================================================//
//
//		arBaseObjectMacro.h
//
//	Provides similar functionality to arBaseObject class, but in macro form to avoid inheritance.
//
//===============================================================================================//
#ifndef AR_BASE_OBJECT_MACRO_H_
#define AR_BASE_OBJECT_MACRO_H_

#include "core/types/types.h"
#include "core/exceptions/exceptions.h"
#include "core/types/arBaseObject.h"

//===============================================================================================//
// Memory Management : Reference counting
//===============================================================================================//
#define AR_REFERENCE_COUNTED_OBJECT(CLASSNAME) \
public:												\
	CLASSNAME* RemoveReference ( void ) {			\
		if ( referenceCount > 0 )					\
			referenceCount--;						\
		else										\
			throw core::InvalidCallException();		\
		return this;								\
	}												\
	CLASSNAME* AddReference ( void )				\
		{ referenceCount++; return this; }			\
	bool HasReference ( void )						\
		{ return (referenceCount != 0); }			\
private:											\
	uint16_t	referenceCount;						\
	//

//===============================================================================================//
// Initialization
//===============================================================================================//
#define AR_REFERENCE_COUNTED_INIT \
	referenceCount(1)

#endif//AR_BASE_OBJECT_MACRO_H_