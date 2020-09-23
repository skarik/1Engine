//===============================================================================================//
//
//		exports.h
// 
// Standard types and defines
//
//===============================================================================================//
#ifndef CORE_EXPORTS_H_
#define CORE_EXPORTS_H_

// Visual Studio specific options
#ifdef _MSC_VER
// Treat unused destructors as error
#	pragma warning (error: 4150)

// Disable certain security warnings
#	ifndef _CRT_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_WARNINGS
#	endif
#	ifndef _CRT_SECURE_NO_DEPRECATE
#	define _CRT_SECURE_NO_DEPRECATE
#	endif
#	ifndef _SCL_SECURE_NO_DEPRECATE
#	define _SCL_SECURE_NO_DEPRECATE
#	endif

// Treat float truncation as an error
//#	pragma warning (error: 4244)
//#	pragma warning (error: 4305)
#endif

// Include NULL defines (with C++12)
#ifdef _WIN32
#	if _MSC_VER > 1500
#		ifndef NULL
#			define NULL (nullptr)
#		else
#			undef NULL
#			define NULL (nullptr)
#		endif
#	else
#		ifndef NULL
#		define NULL (0)
#		endif
#	endif
#else
#	if __cplusplus > 199711L
#		ifndef NULL
#			define NULL (nullptr)
#		else
#			undef NULL
#			define NULL (nullptr)
#		endif
#	else
#		ifndef NULL
#			define NULL (0)
#		endif
#	endif
#endif
// Include NIL define
#ifndef NIL
#	define NIL (0)
#endif

// Include the fixed-size integer types
#if defined(_MSC_VER)
// Define 32bit number if not defined
#		if _MSC_VER > 1500
#			include <stdint.h>
#		else
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

typedef __int8  int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
#		endif
#endif
// Other compilers are sensible and contain the proper types:
#if defined(__clang__)
#			include <stdint.h>
#endif

// Define alignment if not defined
//#		if _MSC_VER > 1700
//#			define ALIGNAS(a) alignas( a )
//#		else
//#			define ALIGNAS(a) __declspec( align( a ) ) 
//#		endif
#		define ALIGNAS(a)

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;

#include "float.h"

// Define the FORCE_INLINE macro
#if		defined(_MSC_VER)
#	define FORCE_INLINE __forceinline
#elif	defined(__clang__)
#	define FORCE_INLINE __attribute__((always_inline)) inline
#endif

// Define the AR API macros
#if		defined(_MSC_VER)
// Import/export
#	define AR_IMPORT __declspec(dllimport)
#	define AR_EXPORT __declspec(dllexport)
// Calls
#	define AR_CALL __cdecl
#	define AR_FASTCALL __fastcall
#elif	defined(__clang__)
// Import/export
#	define AR_IMPORT __attribute__ ((dllimport))
#	define AR_EXPORT __attribute__ ((dllexport))
// Calls
#	define AR_CALL 
#	define AR_FASTCALL 
#endif

// Module types
#ifndef _ENGINE_DEPLOY
#	define DEPLOY_API AR_IMPORT
#else
#	define DEPLOY_API AR_EXPORT
#endif

#ifndef _ENGINE_CORE
#	define CORE_API AR_IMPORT
#else
#	define CORE_API AR_EXPORT
#endif

#ifndef _ENGINE_AUDIO
#	define AUDIO_API AR_IMPORT
#else
#	define AUDIO_API AR_EXPORT
#endif

#ifndef _ENGINE_PHYSICAL
#	define PHYS_API AR_IMPORT
#else
#	define PHYS_API AR_EXPORT
#endif

#ifndef _ENGINE_ENGINE
#	define ENGINE_API AR_IMPORT
#else
#	define ENGINE_API AR_EXPORT
#endif

#ifndef _ENGINE_RENDERER
#	define RENDER_API AR_IMPORT
#else
#	define RENDER_API AR_EXPORT
#endif

#ifndef _ENGINE_GPU_API
#	define GPUW_API AR_IMPORT
#	ifdef _ENGINE_RENDERER
#		define GPUW_EXLUSIVE_API AR_IMPORT
#	else
#		define GPUW_EXLUSIVE_API
#	endif
#else
#	define GPUW_API AR_EXPORT
#	define GPUW_EXLUSIVE_API AR_EXPORT
#endif

#ifndef _ENGINE_RENDER2D
#	define RENDER2D_API AR_IMPORT
#else
#	define RENDER2D_API AR_EXPORT
#endif

#ifndef _ENGINE_ENGINE_COMMON
#	define ENGCOM_API AR_IMPORT
#else
#	define ENGCOM_API AR_EXPORT
#endif

#ifndef _ENGINE_ENGINE2D
#	define ENGINE2D_API AR_IMPORT
#else
#	define ENGINE2D_API AR_EXPORT
#endif

#ifndef _ENGINE_GAME_CORE
#	define GAME_API AR_IMPORT
#else
#	define GAME_API AR_EXPORT
#endif

// Define float assertion helper
#define VALID_FLOAT(a) (( (a)==(a) )&&( ((a) <= FLT_MAX) && ((a) >= -FLT_MAX) ))
// Define assertion
#ifdef _WIN32
#	ifdef _ENGINE_DEBUG
#		include "core/os.h"
// Special assert definition for engine debug
extern "C" {
	_CRTIMP void __cdecl _wassert(_In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line);
}
#		define fnl_assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0) )
#	else
// Not debug mode
#		include "core/os.h"
#		define fnl_assert(_Expression)     ((void)0)
#	endif
#endif
// Include SSE2 functions
#ifdef _WIN32
# include <xmmintrin.h>
#endif

#endif//CORE_EXPORTS_H_