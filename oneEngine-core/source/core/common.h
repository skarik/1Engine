
#ifndef _THE_WON_ENGINE_COMMON___CORE__
#define _THE_WON_ENGINE_COMMON___CORE__

// == DEPRECIATED DEFINITION ==
#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

// == ENGINE STRING USAGE ==
#include <string>
// Using string
using std::string;

// == ENGINE EXCEPTION ==
//#include "EngineExceptions.h"
#include "exceptions/exceptions.h"

// == CLASS PROTOTYPES ==
class CGameBehavior;
class CGameObject;
class CRenderableObject;

// == GLOBAL INSTANTIATION TEMPLATE ==
// USED FOR CLASS REGISTRATION SYSTEM
template<typename T> T * _instantiate( void ) { return new T; }
//template<typename T> CGameBehavior * _instGameBehavior( void ) { return new T; }

//typedef CGameBehavior*(*_instantiationFunction)(void);

#include "callback_basics.h"

template<typename CastTo, typename CastFrom>
FORCE_INLINE CastTo arcast( CastFrom value )
{
#ifdef _ENGINE_DEBUG
	return dynamic_cast<CastTo>(value);
#else
	return static_cast<CastTo>(value);
#endif
}


#ifdef _ENGINE_SAFE_CHECK_
#undef _ENGINE_SAFE_CHECK_
#endif

#endif//_THE_WON_ENGINE_COMMON___CORE__