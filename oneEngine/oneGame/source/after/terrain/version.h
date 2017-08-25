// after/terrain/version.h
// Terrain system version identifier.
//
// Previous iterations used the value TERRA_SYSTEM_VERSION.
// This value has been replaced with a 2-value version, TERRAIN_VERSION_MAJOR and TERRAIN_VERSION_MINOR.
// TERRA_SYSTEM_VERSION still exists, but only refers to the MAJOR version.
//
#ifndef _AFTER_TERRAIN_VERSION_H_
#define _AFTER_TERRAIN_VERSION_H_
#	define TERRAIN_VERSION_MAJOR	(11<<16)
#	define TERRAIN_VERSION_MINOR	0x01
#	define TERRA_SYSTEM_VERSION	TERRAIN_VERSION_MAJOR
#endif//_AFTER_TERRAIN_VERSION_H_