// armodule.h
// Provides macro definition for building an object that synchronizes module singletons to external pointers.

#ifndef _C_AR_MODULE_H_
#define _C_AR_MODULE_H_

// AR Module types
class CGameSettings;
class CPhysics;

#define ARMODULE_DEFINITION Sync ( CGameSettings* , CPhysics* )
#define ARMODULE_SYNC(T,P) if ((P)) { T::SetActive((P)); }

#endif//_C_AR_MODULE_H_