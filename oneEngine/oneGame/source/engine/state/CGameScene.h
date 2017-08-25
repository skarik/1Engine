//===============================================================================================//
//
//		class CGameScene
//
// CGameScene is the object that controls what objects actually appear in the game
// Scenes create what objects are needs, and really really probably should delete them when they end. They will eventually.
//
// Scenes MUST be created with the "new" operator on the heap. If they are not, they will not function properly.
//
//===============================================================================================//
#ifndef _C_GAME_SCENE_
#define _C_GAME_SCENE_

// Includes
#include <memory>
#include "engine/behavior/CGameBehavior.h"

class CGameState;

class CGameScene
{
private:
	// Cannot copy scenes
	CGameScene ( const CGameScene & ) {};

public:
	// Constructor and Destructor
	ENGINE_API			CGameScene ( void );
	ENGINE_API virtual	~CGameScene ( void );

	// Load next scene
	ENGINE_API static void SceneGoto ( CGameScene* );
public:
	template < class Scene >
	static CGameScene* NewScene ( void )
	{
		CGameScene* p = new Scene;
		return p;
	}

	ENGINE_API static CGameScene* pCurrent;

protected:
	// Load new scene
	virtual void LoadScene ( void ) =0;

	// New scene props
	bool bFreeWorld;

private:
	friend CGameState;
	void Load ( void );
};

#endif