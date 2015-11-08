
// Needed includes
#include "gmsceneMainGame.h"

// Include game settings
#include "core/settings/CGameSettings.h"

// Test object includes (aka dicking around)
//#include "CCubeRenderablePrimitive.h"
//#include "CRotBoxRPrimitive.h"
#include "after/terrain/VoxelTerrain.h"
//#include "CCamera.h"
//#include "CPlayer.h"
//#include "DirectionalLight.h"

//#include "COctreeTerrain.h"

//#include "CSprite.h"

//#include "CIsosphere.h"
//#include "DayAndNightCycle.h"

#include "core/debug/CDebugConsole.h"

//#include "CTestPhysicsFloor.h"
#include "after/entities/test/CTestPhysicsCrate.h"

//#include "CModel.h"
//#include "CSkinnedModel.h"

//#include "CExtendableGameObject.h"
//#include "CConvexCollider.h"

//#include "CTestPhysicsFlare.h"

//#include "CTerrainSamplerEditor.h"
//#include "CParticleEditor.h"

//#include "CParticleEmitter.h"
//#include "CParticleUpdater.h"
//#include "CParticleRenderer.h"

//#include "CMccPlayer.h"

//#include "CPlayerLogbook.h"

#include "after/entities/gametype/gametypeNotDying.h"

#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CScreenSpaceOutlineShader.h"
#include "renderer/object/screenshader/effects/CZoomBloomShader.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"
#include "renderer/object/screenshader/effects/CColorFilterShader.h"

//#include "CParticleEmitterCloud.h"
//#include "CParticleMod_Windmotion.h"

#include "engine/utils/CDeveloperConsole.h"

#include "after/developer/CRegionMapTester.h"

void gmsceneMainGame::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene.\n" );
	
	// Set current save files (this is temporary until the menu is finished)
	/*{
		CGameSettings::Active()->SetPlayerSaveFile( "clara" );
		CGameSettings::Active()->SetWorldSaveFile( "lucra" );
		CGameSettings::Active()->SetTerrainSaveFile( "terra" );
	}*/

	// Hnnnnnnng
	/*{
		CDeveloperConsole* devconsole = new CDeveloperConsole();
		devconsole->RemoveReference();
	}*/

	// Set the gametype
	{
		//CGameType* gametype = new gametypeNotDying();
		//gametype->RemoveReference();
	}

	// Terrain
	{
		CGameBehavior* aTerrain = new CVoxelTerrain;
		//CGameBehavior* aTerrain = new COctreeTerrain;
		aTerrain->RemoveReference();
	}

	// Environment
	{
		// Weather
		/*CWeatherSimulator*	daWeather = new CWeatherSimulator;
		daWeather->RemoveReference();

		CPhysicsWindManager*	daWind = new CPhysicsWindManager;
		daWind->RemoveReference();*/

		// Environment effects
		// is in gametype atm

		// Create cloud particle system
		/*CParticleEmitter* cloud_emitter = new CParticleEmitterCloud ( );
		cloud_emitter->LoadFromFile( ".res\\particlesystems\\smoketest02.pcf" );
		cloud_emitter->rfParticlesSpawned = RangeValue<ftype> ( 25.0f,25.0f );
		cloud_emitter->fMaxParticles = 250.0f;
		cloud_emitter->rfStartSize = RangeValue<ftype> ( 30.0f, 40.0f );
		cloud_emitter->rfEndSize = RangeValue<ftype> ( 30.0f, 40.0f );
		cloud_emitter->rfLifetime = RangeValue<ftype> ( 9.0f, 10.5f );
		cloud_emitter->rfAngularVelocity = RangeValue<ftype> ( -5.0f, 5.0f );
		cloud_emitter->rvLinearDamping = RangeValue<Vector3d> ( Vector3d( 1,1,1 )*0.4f, Vector3d( 1,1,1 )*0.4f );

		cloud_emitter->RemoveReference();

		CParticleUpdater* cloud_updater = new CParticleUpdater ( cloud_emitter );
		cloud_updater->AddModifier( new CParticleMod_Windmotion );
		cloud_updater->RemoveReference();

		CParticleRenderer* cloud_renderer = new CParticleRenderer ( cloud_emitter );
		glMaterial* mat_clouds = new glMaterial ();
		mat_clouds->useLighting = true;
		mat_clouds->isTransparent = true;
		mat_clouds->useAlphaTest = false;
		mat_clouds->useDepthMask = false;
		mat_clouds->diffuse = Color( 1.0f,1,1, 0.9f );
		mat_clouds->loadTexture( ".res\\textures\\particles\\smoke1.png" );
		mat_clouds->iBlendMode = glMaterial::BM_NORMAL;
		mat_clouds->iFaceMode = glMaterial::FM_FRONTANDBACK;
		mat_clouds->setShader( new glShader( ".res\\shaders\\particles\\cloudDiffuse.glsl" ) );
		cloud_renderer->SetMaterial( mat_clouds );
		cloud_renderer->iRenderMethod = CParticleRenderer::P_SHADED_BILLBOARD;*/
	}

	// Screen shader
	{
		CRendererHolder* shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
		shaderHolder->name = "Screen Space Outline Shader Holder";
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

	// Map tester
	/*{
		CRegionMapTester* regionMap = new CRegionMapTester();
		regionMap->RemoveReference();
	}*/

	// Particle Editor
	/*{
		CParticleEditor* aParticleEditor = new CParticleEditor;
		aParticleEditor->RemoveReference();
	}
	*/
	// Player object
	/*{
		CPlayer* aPlayer = new CPlayer;
		aPlayer->transform.position = Vector3d( 16,16,24 );
		aPlayer->transform.SetDirty();
		aPlayer->RemoveReference();
	}*/

	// MCC Player Object
	/*{
		CPlayer* aPlayer = new CMccPlayer;
		aPlayer->transform.position = Vector3d( 16,16,24 );
		aPlayer->transform.SetDirty();
		aPlayer->RemoveReference();
	}*/

	// Sky Cycles
	/*{
		Daycycle* dayCycle = new Daycycle();
		dayCycle->SetTimeOfDay( 60*60*18 ); // Set to midnight
		dayCycle->RemoveReference();
	}*/

	// Camera object
	/*{
		CCamera* aCamera = new CCamera;
		aCamera->RemoveReference();
	}*/

	// Logbook test
	/*{
		CPlayerLogbook*	aLogbook = new CPlayerLogbook;
		aLogbook->RemoveReference();
	}*/

	// Old School Sky Cycles
	/*{
		//CIsosphere* aSphere = new CIsosphere;
		DirectionalLight* dLight = new DirectionalLight;
		dLight->diffuseColor = Color( 0.6f,0.6f,0.6f,1.0f );
		Vector3d dir = Vector3d( 0.1f,0.1f,0.9f ).normal();
		dLight->vLightDir = Color( dir.x,dir.y,dir.z,0.0f );
	}*/

	// Physics Testing
	{
		//CTestPhysicsFloor* aFloor = new CTestPhysicsFloor;
		//CTestPhysicsCrate* aCrate = new CTestPhysicsCrate;
		//aCrate->transform.position.z += 20;
		//aCrate->transform.SetDirty();

		//aCrate = new CTestPhysicsCrate;
		//aCrate->transform.position.x += 2;
		//aCrate->transform.position.y += 3;
		//aCrate->transform.position.z += 18;
		//aCrate->transform.SetDirty();
		/*CTestPhysicsCrate* aCrate;
		for ( int i = 0; i < 120; i += 1 )
		{
			aCrate = new CTestPhysicsCrate;
			aCrate->transform.position.x += rand()%10 - 5 + 16;
			aCrate->transform.position.y += rand()%10 - 5 + 16;
			aCrate->transform.position.z += 95 + rand()%10 + 20;
			aCrate->transform.SetDirty();
		}*/
	}

	// Particle System (Wisp)
	/*{
		glMaterial*	newMat = new glMaterial();
		newMat->useLighting = false;
		newMat->diffuse = Color( 1.0f,1,1 );
		newMat->iBlendMode = glMaterial::BM_ADD;
		newMat->loadTexture( ".res\\textures\\sun.jpg" );
		newMat->releaseOwnership();

		CParticleEmitter* emitter = new CParticleEmitter();
		emitter->rfParticlesSpawned.SetRange( 5.0, 10.0f );
		emitter->fMaxParticles = 50.0f;
		emitter->rfEndSize.SetRange( 0.0f,0.0f );

		emitter->rvVelocity.SetRange( Vector3d( -1,-1,-1 ), Vector3d( 1,1,1 ) );

		CParticleUpdater* updater = new CParticleUpdater( emitter );

		CParticleRenderer* renderer = new CParticleRenderer( emitter );
		renderer->SetMaterial( newMat );
	}*/

	// Particle System (Fluxxflame)
	/*{
		glMaterial*	newMat = new glMaterial();
		newMat->useLighting = false;
		newMat->diffuse = Color( 0.6f,0.8f,1.0f, 1.0f );
		newMat->isTransparent = true;
		newMat->useDepthMask = false;
		newMat->useColors = true;
		//newMat->iBlendMode = glMaterial::BM_ADD;
		//newMat->loadTexture( ".res\\textures\\sun.jpg" );
		newMat->loadTexture( ".res\\textures\\particles\\fluxflame1.png" );
		newMat->releaseOwnership();

		CParticleEmitter* emitter = new CParticleEmitter();
		emitter->vEmitterSize = Vector3d( 0.3f,0.3f,0 );

		emitter->rfParticlesSpawned.SetRange( 14.0f, 28.0f );
		emitter->fMaxParticles = 60.0f;

		emitter->rfLifetime.SetRange( 0.6f,1.0f );

		emitter->rfStartSize.SetRange( 0.4f,0.6f );
		emitter->rfEndSize.SetRange( 0.0f,0.0f );

		emitter->rvVelocity.SetRange( Vector3d( -0.5f,-0.5f,2.4f ), Vector3d( 0.5f,0.5f,4.8f ) );
		emitter->rvAcceleration.SetRange( Vector3d( -1,-1,-1 )*0.2f, Vector3d( 2,2,1 )*0.2f );
		emitter->rvLinearDamping.SetRange( Vector3d( 0.2f,0.2f,0.1f ),Vector3d( 0.2f,0.2f,0.1f ) );

		CParticleUpdater* updater = new CParticleUpdater( emitter );

		CParticleRenderer* renderer = new CParticleRenderer( emitter );
		renderer->SetMaterial( newMat );
	}*/

	/*
	// Terrain sampler editor
	{
		CTerrainSamplerEditor* newSamplerEditor = new CTerrainSamplerEditor();
		newSamplerEditor->RemoveReference();
	}
	*/

	// Testing model loading
	/*{
		for ( int i = 0; i < 1; i += 1 )
		{
			//CModel* aModel = new CModel ( string(".res\\models\\test_acorn.pad") );
			//CModel* aModel = new CModel ( string(".res\\models\\eggcorn.FBX") );
			CModel* aModel = new CModel ( string(".res\\models\\flare01.FBX") );
			aModel->transform.position = Vector3d( 0,-i,i*2 ) + Vector3d( 14,20,11 );
			//aModel->transform.scale = Vector3d( 0.1f, 0.1f, 0.1f ) * 0.2f;
			aModel->transform.scale = Vector3d( 2.1f, 2.1f, 2.1f ) ;

			glMaterial* flareMaterial = new glMaterial;
			//flareMaterial->diffuse = Color( 0.7f,0.4f,0.2f ) * 0.6f;
			//flareMaterial->diffuse.alpha = 1.0f;
			//flareMaterial->emissive = Color( 0,0,0.0f );
			//flareMaterial->loadTexture( ".res\\textures\\flare01.tga" );

			//flareMaterial->emissive = Color( 0,0,0.0f );
			//flareMaterial->diffuse = Color( 1,1,1.0f );

			flareMaterial->diffuse = Color( 1,1,1.0f );
			flareMaterial->emissive = Color( 0.1,0.1,0.1f );
			flareMaterial->loadTexture( ".res\\textures\\flare01.tga" );
			flareMaterial->setShader( new glShader( ".res\\shaders\\test.glsl" ) );

			aModel->SetMaterial( flareMaterial );
			flareMaterial->releaseOwnership();
			//aModel->transform.rotation = Vector3d( 90,15,0 );
		}

		for ( int i = 0; i < 1; i += 1 )
		{
			//CModel* aModel = new CModel ( string(".res\\models\\test_acorn.pad") );
			//CModel* aModel = new CModel ( string(".res\\models\\eggcorn.FBX") );
			CModel* aModel = new CModel ( string(".res\\models\\flare01.FBX") );
			aModel->transform.position = Vector3d( 0,-i,i*2 ) + Vector3d( 14,18,11 );
			//aModel->transform.scale = Vector3d( 0.1f, 0.1f, 0.1f ) * 0.2f;
			aModel->transform.scale = Vector3d( 2.1f, 2.1f, 2.1f ) ;

			glMaterial* flareMaterial = new glMaterial;
			//flareMaterial->diffuse = Color( 0.7f,0.4f,0.2f ) * 0.6f;
			//flareMaterial->diffuse.alpha = 1.0f;
			//flareMaterial->emissive = Color( 0,0,0.0f );
			//flareMaterial->loadTexture( ".res\\textures\\flare01.tga" );

			flareMaterial->emissive = Color( 0,0,0.0f );
			flareMaterial->diffuse = Color( 1,1,1.0f );

			//flareMaterial->diffuse = Color( 0,0,0.0f );
			//flareMaterial->emissive = Color( 1,1,1.0f );
			//flareMaterial->loadTexture( ".res\\textures\\flare01.tga" );

			aModel->SetMaterial( flareMaterial );
			flareMaterial->releaseOwnership();
			//aModel->transform.rotation = Vector3d( 90,15,0 );
		}

		CTestPhysicsFlare* aFlare = new CTestPhysicsFlare ();
		aFlare->transform.position = Vector3d( 14,18,11 );
		aFlare->transform.SetDirty();
	}*/
	
	// Natsca Test Model
	/*{
		CModel* aModel = new CSkinnedModel ( string(".res\\models\\natsca2012.pad") );
		//aModel->transform.position = Vector3d( 14,18,11 );
		aModel->transform.position = Vector3d( 14,18,29 );

		glMaterial* bodyMat = new glMaterial();
		bodyMat->loadTexture( ".res\\textures\\characters\\natsca\\maintexsm2.jpg" );
		bodyMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		//bodyMat->useSkinning = true;
		aModel->SetMaterial( bodyMat );

		glMaterial* bandageMat = new glMaterial();
		bandageMat->loadTexture( ".res\\textures\\null.jpg" );
		bandageMat->diffuse = Color( 0.9f,0.8f,0.8f );
		bandageMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		//bandageMat->useSkinning = true;
		aModel->SetMeshMaterial( bandageMat, 1, 3 );

		glMaterial* faceMat = new glMaterial();
		faceMat->loadTexture( ".res\\textures\\characters\\natsca\\facetexsm.jpg" );
		faceMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		//faceMat->useSkinning = true;
		aModel->SetMeshMaterial( faceMat, 1, 4 );

		glMaterial* clothMat = new glMaterial();
		clothMat->loadTexture( ".res\\textures\\null.jpg" );
		clothMat->diffuse = Color( 0.4f,0.3f,0.2f );
		clothMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		//clothMat->useSkinning = true;
		aModel->SetMeshMaterial( clothMat, 1, 5 );

		glMaterial* eyeMat = new glMaterial();
		eyeMat->loadTexture( ".res\\textures\\characters\\natsca\\eye_texture.jpg" );
		eyeMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		aModel->SetMeshMaterial( eyeMat, 1, 1 );
		aModel->SetMeshMaterial( eyeMat, 1, 2 );

		//aModel->GetAnimation()->Play( "default" );
		aModel->GetAnimation()->Play( "ragdoll" );

		//aModel->renderSettings.fOutlineWidth = 0.06f;
		//aModel->renderSettings.cOutlineColor = Color( 1.0f,0,0, 1 );
	}

	// Clara Test Model
	{
		CModel* aModel = new CSkinnedModel ( string(".res\\models\\character\\clara.pad") );

		aModel->transform.position = Vector3d( 18,14,29 );

		aModel->GetAnimation()->Play( "ref" );
	}*/


	// Natsca Test Model
	/*{
		CModel* aModel = new CSkinnedModel ( string(".res\\models\\natsca.pad") );
		//aModel->transform.position = Vector3d( 14,18,11 );
		aModel->transform.position = Vector3d( 4,18,29 );

		glMaterial* bodyMat = new glMaterial();
		bodyMat->loadTexture( ".res\\textures\\characters\\natsca\\maintexsm2.jpg" );
		bodyMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		//bodyMat->useSkinning = true;
		aModel->SetMaterial( bodyMat );

		glMaterial* bandageMat = new glMaterial();
		bandageMat->loadTexture( ".res\\textures\\null.jpg" );
		bandageMat->diffuse = Color( 0.9f,0.8f,0.8f );
		bandageMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		//bandageMat->useSkinning = true;
		aModel->SetMeshMaterial( bandageMat, 1, 3 );

		glMaterial* faceMat = new glMaterial();
		faceMat->loadTexture( ".res\\textures\\characters\\natsca\\facetexsm.jpg" );
		faceMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		//faceMat->useSkinning = true;
		aModel->SetMeshMaterial( faceMat, 1, 4 );

		glMaterial* clothMat = new glMaterial();
		clothMat->loadTexture( ".res\\textures\\null.jpg" );
		clothMat->diffuse = Color( 0.4f,0.3f,0.2f );
		clothMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		//clothMat->useSkinning = true;
		aModel->SetMeshMaterial( clothMat, 1, 5 );

		glMaterial* eyeMat = new glMaterial();
		eyeMat->loadTexture( ".res\\textures\\characters\\natsca\\eye_texture.jpg" );
		eyeMat->setShader( new glShader( ".res\\shaders\\defaultDiffuse.glsl" ) );
		aModel->SetMeshMaterial( eyeMat, 1, 1 );
		aModel->SetMeshMaterial( eyeMat, 1, 2 );


		aModel->renderSettings.fOutlineWidth = 0.06f;
		aModel->renderSettings.cOutlineColor = Color( 1.0f,0,0, 1 );

		aModel->GetAnimation()->Play( "default" );
	}*/
	
	// Testing Physics Objects + Items
	/*{
		CExtendableGameObject* testPhysicsObject = new CExtendableGameObject;
		testPhysicsObject->transform.position = Vector3d( 14,18,11 );
		testPhysicsObject->transform.SetDirty();

		CCollider* testCollider = testPhysicsObject->AddComponent( new CConvexCollider( string(".res\\models\\flare01.FBX") ) );
		testPhysicsObject->AddComponent( new CRigidBody( testCollider, testPhysicsObject ) );

		CModel* aModel = (CModel*)testPhysicsObject->AddComponent( new CModel ( string(".res\\models\\flare01.FBX") ) );
		aModel->transform.SetParent( &(testPhysicsObject->transform) );

		glMaterial* flareMaterial = new glMaterial;
		flareMaterial->emissive = Color( 0,0,0.0f );
		flareMaterial->diffuse = Color( 1,1,1.0f );
		flareMaterial->loadTexture( ".res\\textures\\flare01.tga" );
		aModel->SetMaterial( flareMaterial );
		flareMaterial->releaseOwnership();
	}*/

	// Old stuff
	/*{
		//CCubeRenderPrim aPrim [1000];
		//CRotBoxRPrimitive aPrim [100];
		//CCamera aCamera;
		
		//CCubeRenderablePrimitive aBox( 2.5,2.5,2.5 );
		//aBox.transform.position = aCamera.transform.position + Vector3d( 4, 0, 0 );
		CVoxelTerrain aTerrain;

		CPlayer aPlayer;
		while ( Raytracer::BoxCollides( CCubic( aPlayer.transform.position-Vector3d(0.5f,0.5f,1.5f), Vector3d( 1,1,6.1f ) ), 1|2|4 ) )
		{
			aPlayer.transform.position.z += 2.0f;
		}
		aPlayer.transform.position = Vector3d( -1,-1,24 );

		CIsosphere aSphere;
		//CSprite aSprite;

		glEnable( GL_LIGHTING );
		float lighting [4] = {0.2f,0.15f,0.3f,1.0f};
		float diffuse [4] = {0.6f,0.6f,0.6f,1.0f};
		float pos [4] = {0.1f,0.1f,0.9f,0.0f};
		glLightfv( GL_LIGHT0, GL_AMBIENT, lighting );
		glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );	
		glLightfv( GL_LIGHT0, GL_POSITION, pos );
		glEnable( GL_LIGHT0 );
		DirectionalLight dLight;
		dLight.diffuseColor = Color( 0.6f,0.6f,0.6f,1.0f );
		Vector3d dir = Vector3d( 0.1f,0.1f,0.9f ).normal();
		dLight.vLightDir = Color( dir.x,dir.y,dir.z,0.0f );
	}*/
}