
#include "sceneCloudGame.h"

#include "core/debug/CDebugConsole.h"
#include "core/settings/CGameSettings.h"
#include "engine/utils/CDeveloperConsole.h"

#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CScreenSpaceOutlineShader.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"
#include "renderer/object/screenshader/effects/CZoomBloomShader.h"
#include "renderer/object/screenshader/effects/CColorFilterShader.h"
#include "renderer/object/screenshader/effects/CTestViewShader.h"

#include "renderer/object/util/CLoadScreenInjector.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/material/glMaterial.h"
#include "renderer/camera/CCamera.h"

#include "engine-common/entities/CPlayer.h"
#include "engine-common/network/playerlist.h"

#include "after/entities/world/environment/CEnvironmentEffects.h"
#include "after/entities/world/environment/DayAndNightCycle.h"

#include "cloud/entities/CCloudPlayer.h"
#include "cloud/entities/CCloudEnemy.h"
#include "cloud/entities/CCloudAI.h"
#include "cloud/entities/CPersonality.h"
#include "cloud/entities/CPersonalityFactory.h"

void sceneCloudGame::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene. (instance of sceneCloudGame)\n" );

	// Create load screen
	CLoadScreenInjector* loadscreen = new CLoadScreenInjector();
	//loadscreen->StepScreen();

	{	// Create console visual and leave it floating in the engine
		Engine::CDeveloperConsole* devconsole = new Engine::CDeveloperConsole();
		devconsole->RemoveReference();
	}

	/*{	// Go to _debugging for debugging
		CGameSettings::Active()->SetRealmSaveTarget( "_debugging" );
		CGameSettings::Active()->SetWorldSaveTarget( "terra" );
		CGameSettings::Active()->SetPlayerSaveTarget( "clara" );
	}
	loadscreen->StepScreen();*/
	
	{	// Precache the flat basecase model first
		//CModel* precache = new CModel( "models/cube.FBX" );
		//precache->transform.position = Vector3d(0,0,0);
		//precache->transform.position = Vector3d::zero;
		//precache->transform.scale = Vector3d(1,1,1);
		//delete precache;
	}
	//loadscreen->StepScreen();

	if ( true )
	{	// Camera control
		CActor* player = new CCloudPlayer();
		player->transform.position = Vector3d( -4,-4,6 );
		player->transform.SetDirty();
		player->RemoveReference();
		// Quick hack for adding player to the player list
		Network::AddPlayerActor(player,-1);
	}
	else
	{
		CCamera* camera = new CCamera();
		CRenderCameraHolder* holder = new CRenderCameraHolder(camera);
		holder->RemoveReference();
	}
	//loadscreen->StepScreen();

	if ( true )
	{	// World backgrounds
		//CGameBehavior* effects = new CEnvironmentEffects();
		CGameBehavior* effects = new Daycycle();
		//effects->active = false;
		effects->RemoveReference();
	}
	//loadscreen->StepScreen();

	// enemy test
	if ( true )
	{
		CCloudEnemy* enemy;
		CCloudAI *brain;

		//Make some Vanguards (0)
		for (int x = 0; x < 5; x++)
		{
			enemy = new CCloudEnemy();
			enemy->transform.position = Vector3d( 100, 10 * x - 20, 10);
			brain = new CCloudAI(enemy, 0);
			enemy->SetAIPointer(brain);
			enemy->RemoveReference();
			brain->RemoveReference();
		}

		//Make 6 groups of Mobs (2)
		//Front
		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				enemy = new CCloudEnemy();
				enemy->transform.position = Vector3d(100, 10 * x - 20, 10 * y - 20);
				brain = new CCloudAI(enemy, 2);
				enemy->SetAIPointer(brain);
				enemy->RemoveReference();
				brain->RemoveReference();
			}
		}

		//Above
		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				enemy = new CCloudEnemy();
				enemy->transform.position = Vector3d(10 * x - 20, 10 * y - 20, 100);
				brain = new CCloudAI(enemy, 2);
				enemy->SetAIPointer(brain);
				enemy->RemoveReference();
				brain->RemoveReference();
			}
		}

		//Below
		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				enemy = new CCloudEnemy();
				enemy->transform.position = Vector3d(10 * x - 20, 10 * y - 20, -100);
				brain = new CCloudAI(enemy, 2);
				enemy->SetAIPointer(brain);
				enemy->RemoveReference();
				brain->RemoveReference();
			}
		}

		//Right
		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				enemy = new CCloudEnemy();
				enemy->transform.position = Vector3d(10 * x - 20, 100, 10 * y - 20);
				brain = new CCloudAI(enemy, 2);
				enemy->SetAIPointer(brain);
				enemy->RemoveReference();
				brain->RemoveReference();
			}
		}

		//Left
		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				enemy = new CCloudEnemy();
				enemy->transform.position = Vector3d(10 * x - 20, -100, 10 * y - 20);
				brain = new CCloudAI(enemy, 2);
				enemy->SetAIPointer(brain);
				enemy->RemoveReference();
				brain->RemoveReference();
			}
		}

		//Back
		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				enemy = new CCloudEnemy();
				enemy->transform.position = Vector3d(-100, 10 * x - 20, 10 * y - 20);
				brain = new CCloudAI(enemy, 2);
				enemy->SetAIPointer(brain);
				enemy->RemoveReference();
				brain->RemoveReference();
			}
		}

		//WILDCARDS
		enemy = new CCloudEnemy();
		enemy->transform.position = Vector3d( -75, -75, -75);
		brain = new CCloudAI(enemy, 2);
		enemy->SetAIPointer(brain);
		enemy->RemoveReference();
		brain->RemoveReference();

		enemy = new CCloudEnemy();
		enemy->transform.position = Vector3d(75, 75, 75);
		brain = new CCloudAI(enemy, 2);
		enemy->SetAIPointer(brain);
		enemy->RemoveReference();
		brain->RemoveReference();
	}
	//loadscreen->StepScreen();

	//{	// World
	//	glMaterial* material = new glMaterial;
	//	material->loadFromFile( "debug_terra" );

	//	CModel* model;
	//	model = new CModel( "models/debug/parkourtest01.FBX" );
	//	model->transform.position = Vector3d( 0,0,-8.0f );
	//	model->SetMaterial( material );
	//	/*CExtendableGameObject* go = new CExtendableGameObject();
	//	CCollider* collider;
	//	CRigidBody* body;
	//	{
	//		
	//		go->AddComponent( model );
	//		collider = new CStaticMeshCollider( model->GetModelData(0) );
	//		go->AddComponent( collider );
	//		body = new CRigidBody ( collider, NULL );
	//		body->SetPosition( model->transform.position );
	//		body->SetCollisionLayer( Layers::PHYS_LANDSCAPE );
	//		body->SetMotionType( physMotion::MOTION_FIXED );
	//		go->AddComponent( body );
	//	}
	//	go->RemoveReference();*/
	//	material->removeReference();
	//}
	//loadscreen->StepScreen();

	if ( true )
	{	// Create screen shaders
		CRendererHolder* shaderHolder;
		if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD )
		{
			shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
			shaderHolder->name = "Screen space outline shader Holder";
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CZoomBloomShader() );
			shaderHolder->name = "Zoom Bloom Shader Holder";
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CBloomShader() );
			shaderHolder->name = "Bloom Shader Holder";
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CColorFilterShader() );
			shaderHolder->name = "Color Filter Shader Holder";
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CTestViewShader() );
			shaderHolder->name = "Testview Shader Holder";
			shaderHolder->RemoveReference();
		}
		else
		{
			shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
			shaderHolder->name = "Screen space outline shader Holder";
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CBloomShader() );
			shaderHolder->name = "Bloom Shader Holder";
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CColorFilterShader() );
			shaderHolder->name = "Color Filter Shader Holder";
			shaderHolder->RemoveReference();
		}
	}
	//loadscreen->StepScreen();

	delete loadscreen;
}