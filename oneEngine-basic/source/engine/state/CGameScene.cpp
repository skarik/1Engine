
#include "CGameScene.h"
#include "core/debug/CDebugConsole.h"
#include "CGameState.h"

#include "engine/utils/CDeveloperConsole.h"

// ==Static Variables==
CGameScene* CGameScene::pCurrent = NULL;

// ==Constructor==
CGameScene::CGameScene ( void )
{
	/*if ( pCurrent == NULL )
	{
		pCurrent = this;
	}
	else
	{*/
		debug::Console->PrintMessage( "Game Scene created, waiting for load command.\n" );
	//}

	bFreeWorld = true;
}

// ==Destructor==
CGameScene::~CGameScene ( void )
{


}

// ==Load Next Scene==
void CGameScene::SceneGoto ( CGameScene* pNewScene )
{
	/*if ( pCurrent != NULL )
	{
		if ( pCurrent->bFreeWorld )
		{
			CGameState::pActive->CleanWorld();
			//cout << "...Simulating step...";
			//CGameState::pActive->Update();
			//CGameState::pActive->LateUpdate();
			//CGameState::pActive->CleanWorld();
		}
		delete pCurrent;
	}
	pCurrent = pNewScene;
	pCurrent->LoadScene();*/
	CGameState::Active()->SetNextScene( pNewScene );
}



void CGameScene::Load ( void )
{
	// Load scene and set current scene to this
	LoadScene();
	pCurrent = this;

	// Look through the created objects and look for developer console
	if ( CGameState::Active()->FindFirstObjectWithTypename( "CDeveloperConsole" ) == NULL )
	{
		Engine::CDeveloperConsole* devconsole = new Engine::CDeveloperConsole();
		devconsole->RemoveReference();
	}
}
