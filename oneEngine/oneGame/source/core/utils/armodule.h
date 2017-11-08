//===============================================================================================//
//
//		armodule.h
//
// Provides macro definition for building an object that synchronizes module singletons to
//	external pointers.
//
// Currently unused with the current linking scheme.
//
//===============================================================================================//
#ifndef C_AR_MODULE_H_
#define C_AR_MODULE_H_

// AR Module types
class CGameSettings;
class CPhysics;

#define ARMODULE_DEFINITION Sync ( CGameSettings* , CPhysics* )
#define ARMODULE_SYNC(T,P) if ((P)) { T::SetActive((P)); }

#endif//C_AR_MODULE_H_