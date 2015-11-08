//===============================================================================================//
//		CGameHandle
// Class meant to replace usage of CGameBehavior**
// Do not create this manually. Use CGameBehavior::GetHandle() to get an instance of this for you.
// This should not replace the usage of pointers, as it is slow compared to pointers
//===============================================================================================//
#ifndef _ENGINE_C_GAME_HANDLE_H_
#define _ENGINE_C_GAME_HANDLE_H_

#include "CGameBehavior.h"

//===============================================================================================//
// CGameHandle
//===============================================================================================//
class CGameHandle
{
public:
	CGameHandle ( CGameBehavior* );

	// Conversion
	ENGINE_API operator CGameBehavior*();
	// Dereference
	ENGINE_API CGameBehavior* operator-> ( void );
	// Check
	ENGINE_API bool valid ( void );

private:
	CGameBehavior*	behavior;
	gameid_t		id;
	netid_t			netid;
};


#endif//_ENGINE_C_GAME_HANDLE_H_