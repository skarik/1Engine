//===============================================================================================//
//
//		types/modes.h
// 
// Macros for the current build mode & better granularity.
//
//===============================================================================================//
#ifndef CORE_MODES_H_
#define CORE_MODES_H_

#if _ENGINE_DEBUG
#	define BUILD_DEVELOPMENT 1
#else
#	define BUILD_RELEASE 1
#endif

#endif//CORE_MODES_H_