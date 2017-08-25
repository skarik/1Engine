//===============================================================================================//
//
//		baseType.h
//
// Provides enumerations for engine base object types. Enumerations are bitfields in the case of
// multiple inheritance.
//
//===============================================================================================//
#ifndef CORE_EXT_BASE_TYPES_H_
#define CORE_EXT_BASE_TYPES_H_

#include "core/types.h"

namespace core
{
	enum arBaseType : uint8_t
	{
		kBasetypeVoidPointer		= 0,
		kBasetypeIPrMotion			= 1,
		kBasetypeGameBehavior		= 2,
		kBasetypeRrLogicObject		= 4,
		kBasetypeRrRenderObject		= 8
	};
}

#endif//CORE_EXT_BASE_TYPES_H_