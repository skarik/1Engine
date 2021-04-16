#ifndef CORE_EXT_H_
#define CORE_EXT_H_

#include "core/types.h"
#include <functional>

namespace core
{
	CORE_API void OnApplicationStartup ( void );
	CORE_API void OnApplicationEnd ( void );

	CORE_API void _RegisterOnApplicationStartup( std::function<void()> Fn );
	CORE_API void _RegisterOnApplicationEnd( std::function<void()> Fn );
}

#define REGISTER_ON_APPLICATION_STARTUP(Fn) \
	static struct arApplicationStartupRegistry_##Fn \
	{ \
	public: \
		/* Constructor, adds self to the registry. */ \
		arApplicationStartupRegistry_##Fn() \
		{ \
			core::_RegisterOnApplicationStartup(Fn); \
		} \
	} InstApplicationStartupRegistry_##Fn;
#define REGISTER_ON_APPLICATION_END(Fn) \
	static struct arApplicationEndRegistry_##Fn \
	{ \
	public: \
		/* Constructor, adds self to the registry. */ \
		arApplicationEndRegistry_##Fn() \
		{ \
			core::_RegisterOnApplicationStartup(Fn); \
		} \
	} InstApplicationEndRegistry_##Fn;

#endif//CORE_EXT_H_