
#include "core/debug/Console.h"
#include "core-ext/system/shell/Status.h"
#include "engine/utils/CDeveloperConsole.h"

#include "CGameScene.h"
#include "CGameState.h"

// ==Static Variables==
CGameScene* CGameScene::pCurrent = NULL;

// ==Constructor==
CGameScene::CGameScene ( void )
{
	debug::Console->PrintMessage( "Game Scene created, waiting for load command.\n" );
	bFreeWorld = true;
}

// ==Destructor==
CGameScene::~CGameScene ( void )
{
	//
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
	// Show user that we are currently loading
	core::shell::SetTaskbarProgressState(NIL, core::shell::kTaskbarStateIndeterminate);

	// Load scene and set current scene to this
	LoadScene();
	pCurrent = this;

	// Look through the created objects and look for developer console
	if ( CGameState::Active()->FindFirstObjectWithTypename( "CDeveloperConsole" ) == NULL )
	{
		engine::CDeveloperConsole* devconsole = new engine::CDeveloperConsole();
		devconsole->RemoveReference();
	}

	// Signify loading is done
	core::shell::SetTaskbarProgressValue(NIL, 100, 100);
}
