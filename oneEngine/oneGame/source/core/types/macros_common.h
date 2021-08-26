//===============================================================================================//
//
//		macros_common.h
//
// Contains common macros used for utility cases, such as tight padding.
//
//===============================================================================================//
#ifndef CORE_MACROS_COMMON_H_
#define CORE_MACROS_COMMON_H_

// Visual Studio specific options
#ifdef _MSC_VER
#define	LAYOUT_PACK_TIGHTLY __pragma(pack(push, 1))
#define LAYOUT_PACK_END		__pragma(pack(pop))
#endif

#endif//CORE_MACROS_COMMON_H_