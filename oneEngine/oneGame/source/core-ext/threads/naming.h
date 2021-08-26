#ifndef CORE_EXT_THREAD_NAMING_H_
#define CORE_EXT_THREAD_NAMING_H_

#include "core/types.h"
#include <thread>

namespace core {
namespace threads {

	//	SetThreadName ( thread, name ) : Sets the given thread's debug name.
	CORE_API void			SetThreadName ( std::thread& thread, const char* thread_name );
	//	SetThreadName ( thread, name ) : Sets the calling thread's debug name.
	CORE_API void			SetThisThreadName ( const char* thread_name );

}}

#endif//CORE_EXT_THREAD_NAMING_H_