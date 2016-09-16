
#include "testSceneStaticMesh.h"

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

#include "engine-common/entities/CPlayer.h"
#include "engine-common/network/playerlist.h"

#include "after/entities/world/environment/CEnvironmentEffects.h"
#include "after/entities/world/environment/DayAndNightCycle.h"

void testSceneStaticMesh::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene. (instance of gmsceneCharacterViewer)\n" );

	// Create load screen
	CLoadScreenInjector* loadscreen = new CLoadScreenInjector();
	loadscreen->StepScreen();

	{	// Create console visual and leave it floating in the engine
		Engine::CDeveloperConsole* devconsole = new Engine::CDeveloperConsole();
		devconsole->RemoveReference();
	}

	{	// Go to _debugging for debugging
		CGameSettings::Active()->SetRealmSaveTarget( "_debugging" );
		CGameSettings::Active()->SetWorldSaveTarget( "terra" );
		CGameSettings::Active()->SetPlayerSaveTarget( "clara" );
	}
	loadscreen->StepScreen();

	// Create testing lambda
	auto showResults = [&]( const string& message, void* success )
	{
		std::cout << message << ": ";
		// Print test result
		if ( success == NULL ) {
			Debug::Console->PrintError( "[FAILED]\n" );
		}
		else {
			std::cout << "[PASS]" << std::endl;
		}
	};
	
	{	// Precache the flat basecase model first
		CModel* precache = new CModel( "models/biased_cube.FBX" );
		showResults( "Biased cube simple model load", precache );
		precache->transform.position = Vector3d(0,0,16);
		//precache->transform.position = Vector3d::zero;
		precache->transform.scale = Vector3d(1,1,1);
		//delete precache;
	}
	loadscreen->StepScreen();

	{	// Camera control
		CActor* observer = new CPlayer();
		observer->transform.position = Vector3d( -4,-4,6 );
		observer->transform.SetDirty();
		observer->RemoveReference();
		// Quick hack for adding player to the player list
		Network::AddPlayerActor(observer,-1);
	}
	loadscreen->StepScreen();

	{	// World backgrounds
		//CGameBehavior* effects = new CEnvironmentEffects();
		CGameBehavior* effects = new Daycycle();
		//effects->active = false;
		effects->RemoveReference();
	}
	loadscreen->StepScreen();

	{	// World
		glMaterial* material = new glMaterial;
		material->loadFromFile( "debug_terra" );

		CModel* model;
		model = new CModel( "models/debug/parkourtest01.FBX" );
		model->transform.position = Vector3d( 0,0,-8.0f );
		model->SetMaterial( material );
		/*CExtendableGameObject* go = new CExtendableGameObject();
		CCollider* collider;
		CRigidBody* body;
		{
			
			go->AddComponent( model );
			collider = new CStaticMeshCollider( model->GetModelData(0) );
			go->AddComponent( collider );
			body = new CRigidBody ( collider, NULL );
			body->SetPosition( model->transform.position );
			body->SetCollisionLayer( Layers::PHYS_LANDSCAPE );
			body->SetMotionType( physMotion::MOTION_FIXED );
			go->AddComponent( body );
		}
		go->RemoveReference();*/
		material->removeReference();
	}
	loadscreen->StepScreen();

	{	// Create screen shaders
		CRendererHolder* shaderHolder;
		if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD )
		{
			shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
			shaderHolder->name = "Screen space outline shader Holder";
			showResults( shaderHolder->name, shaderHolder->GetRenderer() );
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CZoomBloomShader() );
			shaderHolder->name = "Zoom Bloom Shader Holder";
			showResults( shaderHolder->name, shaderHolder->GetRenderer() );
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CBloomShader() );
			shaderHolder->name = "Bloom Shader Holder";
			showResults( shaderHolder->name, shaderHolder->GetRenderer() );
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CColorFilterShader() );
			shaderHolder->name = "Color Filter Shader Holder";
			showResults( shaderHolder->name, shaderHolder->GetRenderer() );
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CTestViewShader() );
			shaderHolder->name = "Testview Shader Holder";
			showResults( shaderHolder->name, shaderHolder->GetRenderer() );
			shaderHolder->RemoveReference();
		}
		else
		{
			shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
			shaderHolder->name = "Screen space outline shader Holder";
			showResults( shaderHolder->name, shaderHolder->GetRenderer() );
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CBloomShader() );
			shaderHolder->name = "Bloom Shader Holder";
			showResults( shaderHolder->name, shaderHolder->GetRenderer() );
			shaderHolder->RemoveReference();

			shaderHolder = new CRendererHolder( new CColorFilterShader() );
			shaderHolder->name = "Color Filter Shader Holder";
			showResults( shaderHolder->name, shaderHolder->GetRenderer() );
			shaderHolder->RemoveReference();
		}
	}
	loadscreen->StepScreen();

	delete loadscreen;
}