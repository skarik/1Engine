//===============================================================================================//
//
//		mem.h
// 
// Includes macros and common functions dealing with mem
//
//===============================================================================================//
#ifndef CORE_MEM_
#define CORE_MEM_

#ifndef delete_safe
#	define delete_safe(_ptr) { if ( _ptr ) { delete (_ptr); (_ptr) = NULL; } };
#endif
#ifndef delete_safe_array
#	define delete_safe_array(_ptr) { if ( _ptr ) { delete[] (_ptr); (_ptr) = NULL; } };
#endif
#ifndef delete_safe_decrement
#	define delete_safe_decrement(_ptr) { if ( _ptr ) { (_ptr)->RemoveReference(); delete (_ptr); (_ptr) = NULL; } };
#endif

#endif//CORE_MEM_