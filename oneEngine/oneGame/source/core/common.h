//===============================================================================================//
//
//		common.h
// 
// Old messy common code.
//
//===============================================================================================//
#ifndef CORE_COMMON_MESSY_H_
#define CORE_COMMON_MESSY_H_

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
#include "exceptions/exceptions.h"

// == CLASS PROTOTYPES ==
class CGameBehavior;
class CGameObject;
class CRenderableObject;

// == GLOBAL INSTANTIATION TEMPLATE ==
// USED FOR CLASS REGISTRATION SYSTEM
template<typename T> T * _instantiate( void ) { return new T; }
template<typename T, typename To> To * _instantiate( void ) { return (To*)(new T); }
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

#ifndef offsetof
template <typename T, typename U>
constexpr size_t offsetof_impl(T const* t, U T::* a)
{
	return	(char const*)t - (char const*)&(t->*a) >= 0 ?
			(char const*)t - (char const*)&(t->*a)      :
			(char const*)&(t->*a) - (char const*)t;
}

//	offsetof(Class, Variable)
// Caller must be in scope of the variable.
// The variable must be a C-compatible type. This means only data, no functions, statics, constants, or C++ specific types.
// Returns in bytes the offset of the variable in the given class.
#define offsetof(Type_, Attr_) \
    offsetof_impl((Type_ const*)nullptr, &Type_::Attr_)
#endif

#ifdef _ENGINE_SAFE_CHECK_
#undef _ENGINE_SAFE_CHECK_
#endif

#endif//CORE_COMMON_MESSY_H_