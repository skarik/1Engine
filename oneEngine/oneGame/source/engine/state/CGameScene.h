//===============================================================================================//
//
//		class CGameScene
//
// CGameScene is the object that controls what objects actually appear in the game
// Scenes create what objects are needs, and really really probably should delete them when they end. They will eventually.
//
// Scenes MUST be created with the "new" operator on the heap. If they are not, they will not function properly.
//
// New usage of scenes: they are little payloads of objects. They load up new objects.
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
	ENGINE_API				CGameScene ( void );
	ENGINE_API virtual		~CGameScene ( void );


protected:
	// Loading parameters:

	// Does this scene destroy the current scene when being loaded in?
	bool				bIsAdditive = false;
	// Can this scene be streamed in over time
	bool				bCanBeStreamed = false;

	//	LoadScene() : Called when loading the new scene
	ENGINE_API virtual void	LoadScene ( void ) =0;

	//	GetSceneName() : Called to get the scene
	ENGINE_API virtual const char*
							GetSceneName ( void )
		{ return "Unnamed Scene"; }

public:
	// Load next scene
	ENGINE_API static void	SceneGoto ( CGameScene* );

public:
	template < class Scene >
	static CGameScene* NewScene ( void )
	{
		CGameScene* p = new Scene;
		return p;
	}

	ENGINE_API static CGameScene* pCurrent;

private:
	friend CGameState;
	//	Load() : Called by the engine state when loading.
	void					Load ( void );
};

#define ER_DEFINE_SCENE_NAME( Export, Name ) \
	Export static const char* GetName ( void ) { return Name ; } \
	Export virtual const char* GetSceneName ( void ) override { return Name ; }

#endif