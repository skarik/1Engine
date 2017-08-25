
// Needed includes
#include "gmsceneMenu.h"


#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"

#include "after/entities/menu/front/C_RMainMenu.h"

//#include "after/entities/menu/CTerrainCollisionLoader.h"

#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CScreenSpaceOutlineShader.h"
#include "renderer/object/screenshader/effects/CZoomBloomShader.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"
#include "renderer/object/screenshader/effects/CColorFilterShader.h"


void gmsceneMenu::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene. (instance of gmsceneMenu)\n" );

	// Hnnnnnnng
	{
		Engine::CDeveloperConsole* devconsole = new Engine::CDeveloperConsole();
		devconsole->RemoveReference();
	}

	// Screen shader
	{
		CRendererHolder* shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
		shaderHolder->name = "Screen space outline shader Holder";
		shaderHolder->RemoveReference();

		CRendererHolder* shaderHolder2 = new CRendererHolder( new CZoomBloomShader() );
		shaderHolder2->name = "Zoom Bloom Shader Holder";
		shaderHolder2->RemoveReference();

		CRendererHolder* shaderHolder3 = new CRendererHolder( new CBloomShader() );
		shaderHolder3->name = "Bloom Shader Holder";
		shaderHolder3->RemoveReference();

		CRendererHolder* shaderHolder4 = new CRendererHolder( new CColorFilterShader() );
		shaderHolder4->name = "Color Filter Shader Holder";
		shaderHolder4->RemoveReference();
	}

	// Terrain collision background loader to speed up getting into game
	/*{
		CTerrainCollisionLoader* loader = new CTerrainCollisionLoader();
		loader->RemoveReference();
	}*/

	/*{
		CloudSphere* cloudCycle = new CloudSphere();
		cloudCycle->RemoveReference();
	}*/

	/*CVoxelTerrain* aTerrain = new CVoxelTerrain;
	//CTestPhysicsFloor* aFloor = new CTestPhysicsFloor;
	CTestPhysicsCrate* aCrate = new CTestPhysicsCrate;
	aCrate->transform.position.z += 20;
	aCrate->transform.SetDirty();

	aCrate = new CTestPhysicsCrate;
	aCrate->transform.position.x += 2;
	aCrate->transform.position.y += 3;
	aCrate->transform.position.z += 18;
	aCrate->transform.SetDirty();
	for ( int i = 0; i < 120; i += 1 )
	{
		aCrate = new CTestPhysicsCrate;
		aCrate->transform.position.x += rand()%10 - 5;
		aCrate->transform.position.y += rand()%10 - 5;
		aCrate->transform.position.z += 15 + rand()%10;
		aCrate->transform.SetDirty();
	}*/

	//CPlayer* aPlayer = new CPlayer;
	/*CCamera* aCamera = new CCamera;
	//aPlayer->transform.position = Vector3d( 16,16,24 );
	//aPlayer->transform.SetDirty();

	Daycycle* dayCycle = new Daycycle();

	CMainMenu* aMenu = new CMainMenu;*/

	{
		C_RMainMenu* aMenu = new C_RMainMenu(NULL);
		aMenu->RemoveReference();
	}
}