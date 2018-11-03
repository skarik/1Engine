//===============================================================================================//
//
//		debug.h
//
// some macros used extensively in the larger system to call developers idiots
//
//===============================================================================================//
#ifndef CORE_DEBUG_H_
#define CORE_DEBUG_H_

#include <stdio.h>

#if defined(_MSC_VER)
#	define AR_DEBUG_BREAK() __debugbreak()
#elif defined (__CLANG__)
#	define AR_DEBUG_BREAK() asm("int $3")
#else
#	error Unknown platform (Only MSVC and Clang supported)
#endif

#ifndef _ENGINE_DEBUG
#define ARCORE_ENABLE_ASSERTS 0
#else
#define ARCORE_ENABLE_ASSERTS 1
#endif

#define AR_ERROR_DEAD(fmt, ...)				do { (void)1; } while(0)
#define AR_ASSERT_MSG_DEAD(cond, fmt, ...)	do { (void)(1 ? (void)0 : ((void)(cond)) ); } while(0)

#define AR_ERROR_IMPL(fmt, ...) \
	do { \
		printf(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
		AR_DEBUG_BREAK(); \
	} while(0);
#define AR_ASSERT_MSG_IMPL(cond, fmt, ...) \
	do { \
		if (!(cond)) { \
			printf(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
			AR_DEBUG_BREAK(); \
		} \
	} while(0);

// disable asserts for code analyzers
#if defined(__clang_analyzer__) || defined(__cppcheck__) || defined(_PREFAST_)
#	include <assert.h>
#	define ARCORE_ERROR(cond, fmt, ...)			assert(1)
#	define ARCORE_ASSERT_MSG(cond, fmt, ...)	assert((cond))
#elif ARCORE_ENABLE_ASSERTS
#	define ARCORE_ERROR(fmt, ...)				AR_ERROR_IMPL(fmt, ## __VA_ARGS__)
#	define ARCORE_ASSERT_MSG(cond, fmt, ...)	AR_ASSERT_MSG_IMPL(cond, fmt, ## __VA_ARGS__)
#else
#	define ARCORE_ERROR(fmt, ...)				AR_ERROR_DEAD(fmt, ## __VA_ARGS__)
#	define ARCORE_ASSERT_MSG(cond, fmt, ...)	AR_ASSERT_MSG_DEAD(cond, fmt, ## __VA_ARGS__)
#endif

// lazy assert
#ifndef    ARCORE_ASSERT
#	define ARCORE_ASSERT(cond)	ARCORE_ASSERT_MSG(cond, "assert '%s' failed", #cond)
#endif

#endif//CORE_DEBUG_H_