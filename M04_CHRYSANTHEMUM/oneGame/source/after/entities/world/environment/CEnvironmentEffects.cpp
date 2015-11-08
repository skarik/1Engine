
#include "CEnvironmentEffects.h"
#include "core/math/Math.h"
#include "core/math/random/Random.h"

// Include camera
#include "renderer/camera/CCamera.h"
// Include render settings
#include "renderer/state/Settings.h"
// Include gamestate checking
#include "engine/state/CGameState.h"

// Include zone checkers
#include "after/physics/water/Water.h"
#include "after/physics/wind/WindMotion.h"
#include "after/terrain/Zones.h"
#include "after/types/terrain/TerrainTypes.h"
#include "engine/physics/raycast/Raycaster.h"

// Include day and night cycle
#include "after/entities/world/environment/DayAndNightCycle.h"
// Include the clouds
#include "CloudSphere.h"
// Include inclement weather effects
#include "EnvHeavyWeather.h"

// Include extendable objects
#include "engine/behavior/CExtendableGameObject.h"

// Include particle systems
#include "engine-common/entities/CParticleSystem.h"
#include "renderer/logic/particle/CParticleEmitter.h"
#include "renderer/object/particle/CParticleRenderer.h"
#include "renderer/logic/particle/CParticleUpdater.h"
// Include some specific particle mods
#include "renderer/logic/particle/modifiers/CParticleMod_Windmotion.h"

// Include sound interface
#include "engine/audio/CAudioInterface.h"

// Include weather systems
#include "after/entities/world/environment/weather/CWeatherSimulator.h"
#include "engine-common/entities/physics/wind/CPhysicsWindManager.h"

// Include screen shaders used for environment effects
#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CColorFilterShader.h"

#include "renderer/material/glMaterial.h"

CEnvironmentEffects::CEnvironmentEffects ( void )
	: CGameObject()
{
	pWorldDaycycle = new Daycycle;
	pWorldMooncycle = new Mooncycle;
	pWorldCloudsphere = new CloudSphere;
	
	pHeavyWeatherEffect = NULL;

	fCloudCover = 0;

	InitializeParticleEffects();
	InitializeFogEffects();
	InitializeSoundEffects();

	iBiomeType = Terrain::BIO_GRASSLAND;
	iNextBiomeType = Terrain::BIO_DEFAULT;
	iWeatherType = Terrain::WTH_LIGHT_RAIN;
	iNextWeatherType = Terrain::WTH_NONE;
	fTemperature = 1.0f;
	fTemperatureSmoother = fTemperature;

	m_weatherSim = new CWeatherSimulator;
	m_weatherSim->RemoveReference();

	m_weatherWind = new CPhysicsWindManager;
	m_weatherWind->RemoveReference();

	vLastCameraCheckPosition = Vector3d();
	fCameraCheckTimer = 5.0f;

	pColorFilter = NULL;
}

CEnvironmentEffects::~CEnvironmentEffects ( void )
{
	delete pWorldDaycycle;
	delete pWorldMooncycle;
	delete pWorldCloudsphere;

	delete pHeavyWeatherEffect;

	FreeParticleEffects();
	FreeSoundEffects();
}

void CEnvironmentEffects::OnEnable ( void )
{
	m_weatherSim->active = true;
	m_weatherWind->active = true;
}
void CEnvironmentEffects::OnDisable ( void )
{
	m_weatherSim->active = false;
	m_weatherWind->active = false;
}

// Defaults
void CEnvironmentEffects::Update ( void )
{
	vCameraPosition = CCamera::activeCamera->transform.position;
	iNextBiomeType = Zones.GetTerrainBiomeAt( vCameraPosition );
	iNextWeatherType = WindMotion.GetWeather( vCameraPosition );
	fTemperature = WindMotion.GetTemperatureFast( vCameraPosition );
	UpdateCameraIndoor();
	UpdateParticleEffects();

	fCloudCover = Math.Lerp( Time::deltaTime*0.1f, fCloudCover, WindMotion.GetCloudDensity( vCameraPosition + Vector3d( 0,0,100 ) ) );
	//cout << fCloudCover << endl;
	pWorldCloudsphere->SetCloudDensity( fCloudCover );
	pWorldDaycycle->SetSkyCoverage( std::max<ftype>( 0, std::min<ftype>( 1.2f, (fCloudCover-0.1f)*1.2f ) ) );

	UpdateSoundEffects();
}
void CEnvironmentEffects::UpdateCameraIndoor ( void )
{
	fCameraCheckTimer += Time::deltaTime;
	bCameraIndoorStateChanged = false;
	if ( ((vLastCameraCheckPosition-vCameraPosition).magnitude() > 6.0f)||( fCameraCheckTimer > 4.0f ) )
	{
		fCameraCheckTimer = 0.0f;
		vLastCameraCheckPosition = vCameraPosition;

		// Perform check for indoors via a LOT of raytraces
		int iWallCount = 0;
		int iCeilCount = 0;

		for ( int i = 0; i < 8; ++i )
		{
			Ray castRay ( vCameraPosition, Vector3d( cosf( i*0.785f ), sinf( i*0.785f ), 0 ) );
			RaycastHit hitInfo;
			// First cast to wall
			if ( Raycaster.Raycast( castRay, 60.0f, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ) ) {
				iWallCount += 1;
				// Now cast to the ceiling from those points
				Ray subcastRay ( hitInfo.hitPos + hitInfo.hitNormal, Vector3d::up );
				if ( Raycaster.Raycast( castRay, 20.0f, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ) ) {
					iCeilCount += 1;
				}
				subcastRay.pos = (subcastRay.pos+castRay.pos)/2;
				if ( Raycaster.Raycast( castRay, 40.0f, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) ) ) {
					iCeilCount += 1;
				}
			}
		}
		// Based on number of hits, make us inside
		if ( iWallCount >= 5 && iCeilCount >= 13 ) {
			if ( !bCameraIndoors ) {
				bCameraIndoorStateChanged = true;
				bCameraIndoors = true;
			}
		}
		else {
			if ( bCameraIndoors ) {
				bCameraIndoorStateChanged = true;
				bCameraIndoors = false;
			}
		}
	}
}

void CEnvironmentEffects::PostUpdate ( void )
{
	vCameraPosition = CCamera::activeCamera->transform.position;
	bCameraUnderwater = WaterTester::Get()->PositionInside( vCameraPosition );
	UpdateFogColors();
	UpdateFogEffects();
	UpdateColorFilter();
	iBiomeType		= iNextBiomeType;
	iWeatherType	= iNextWeatherType;
}

// Init fog variables
void CEnvironmentEffects::InitializeFogEffects ( void )
{
	// Color multipliers
	cFogMultiplier	= Color(1,1,1,1);
	cFogOffset		= Color(0,0,0,0);
	fFogEndMultiplier	= 1.0f;
	cAmbientMultiplier	= Color(1,1,1,1);

	fFogLerp		= 0.0f;
	cFogMultiplier_Start	= Color(1,1,1,1);
	cFogOffset_Start		= Color(0,0,0,0);
	fFogEndMultiplier_Start = 1.0f;
	cAmbientMultiplier_Start= Color(1,1,1,1);

}
// Update fog colors (overridable)
void CEnvironmentEffects::UpdateFogColors ( void )
{
	Color cFogOffset_Target			= Color( 0,0,0,0 );
	Color cFogMultiplier_Target		= Color( 1,1,1,1 );
	ftype fFogEndMultiplier_Target	= 1.0f;
	Color cAmbientMultiplier_Target = Color( 1,1,1,1 );
	bool bHadChange = false;

	// Set Biome-based fog colors (plus seasons)
	if (( iNextBiomeType != iBiomeType )||( iWeatherType != iNextWeatherType ))
	{
		bHadChange = true;
	}
	switch ( iNextBiomeType )
	{
	case Terrain::BIO_DESERT:
		cFogMultiplier_Target = Color( 0.93f,0.87f,0.63f ) * 1.3f;
		cFogOffset_Target = Color( 0.04f,0.03f,0.0f );
		fFogEndMultiplier_Target = 0.8f;
		cAmbientMultiplier_Target = Color( 1.2f,1.18f,1.15f,1.0f );
		break;
	case Terrain::BIO_GRASS_DESERT:
		cFogMultiplier_Target = Color( 0.93f,0.77f,0.73f ) * 1.3f;
		cFogOffset_Target = Color( 0.03f,0.02f,0.0f );
		fFogEndMultiplier_Target = 0.8f;
		cAmbientMultiplier_Target = Color( 1.2f,1.13f,1.13f,1.0f );
		break;
	case Terrain::BIO_THE_EDGE:
		cFogMultiplier_Target = Color( 0.80f,0.78f,0.98f );
		cFogOffset_Target = Color( -0.03f, -0.04f, -0.03f );
		fFogEndMultiplier_Target = 0.5f;
		cAmbientMultiplier_Target = Color( 0.85f,0.8f,0.9f );
		break;
	case Terrain::BIO_COLD_DESERT:
	case Terrain::BIO_TAIGA:
	case Terrain::BIO_TUNDRA:
		cFogMultiplier_Target = Color( 0.90f,0.93f,1.00f );
		cFogOffset_Target = Color( 0.00f, 0.04f, 0.05f );
		fFogEndMultiplier_Target = 0.8f;
		cAmbientMultiplier_Target = Color( 0.9f,0.9f,1.0f );
		break;
	case Terrain::BIO_S_TUNDRA:
		cFogMultiplier_Target = Color( 0.90f,0.93f,1.00f );
		cFogOffset_Target = Color( 0.06f, 0.06f, 0.09f );
		fFogEndMultiplier_Target = 0.5f;
		cAmbientMultiplier_Target = Color( 0.6f,0.63f,0.72f );
		break;
	case Terrain::BIO_EVERGREEN:
	case Terrain::BIO_SAVANNA:
		cFogMultiplier_Target = Color( 1.08f,1.06f,0.94f );
		fFogEndMultiplier_Target = 0.9f;
		cAmbientMultiplier_Target = Color( 1.08f,1.06f,0.94f );
		break;
	/*case Terrain::BIO_GRASSLAND:
	case Terrain::BIO_TEMPERATE:
		fFogEndMultiplier_Target = 0.9f;
		cAmbientMultiplier_Target = Color( 1.05f,1.04f,0.95f );
		break;*/
	case Terrain::BIO_TROPICAL:
	case Terrain::BIO_RAINFOREST:
		cFogMultiplier_Target = Color( 1.05f,1.05f,0.95f );
		fFogEndMultiplier_Target = 0.9f;
		cAmbientMultiplier_Target = Color( 0.97f,1.06f,0.94f );
		break;
	case Terrain::BIO_MOIST:
	case Terrain::BIO_SWAMP:
		cFogMultiplier_Target = Color( 0.97f,1.06f,0.97f );
		fFogEndMultiplier_Target = 0.8f;
		cAmbientMultiplier_Target = Color( 0.87f,0.98f,0.85f );
		break;
	}
	switch ( iNextWeatherType )
	{
	case Terrain::WTH_HEAVY_RAIN:
	case Terrain::WTH_HEAVY_RAIN_THUNDER:
	case Terrain::WTH_HEAVY_SNOW:
	case Terrain::WTH_STORM_SNOW:
		fFogEndMultiplier_Target *= 0.2f;
		cFogMultiplier_Target = cFogMultiplier_Target * 0.85f;
		break;
	case Terrain::WTH_DUST_STORM:
		fFogEndMultiplier_Target *= 0.06f;
		cFogMultiplier_Target = cFogMultiplier_Target * 1.05f;
		//cAmbientMultiplier_Target = Color( 1.12f,0.97f,0.94f )*0.6f;
		cAmbientMultiplier_Target = Color( 1.12f,0.97f,0.94f )*-0.2f;
		break;
	case Terrain::WTH_LIGHT_RAIN:
	case Terrain::WTH_LIGHT_RAIN_THUNDER:
	case Terrain::WTH_LIGHT_SNOW:
		fFogEndMultiplier_Target *= 0.5f;
		cFogMultiplier_Target = cFogMultiplier_Target * 0.93f;
		break;
	case Terrain::WTH_FOG_LIGHT:
		fFogEndMultiplier_Target *= 0.16f;
		cFogMultiplier_Target.Lerp( Color(0.5f,0.5f,0.5f), 0.2f );
		break;
	}

	if ( bHadChange )
	{
		fFogLerp = 0;
		cFogMultiplier_Start	= cFogMultiplier;
		cFogOffset_Start		= cFogOffset;
		fFogEndMultiplier_Start = fFogEndMultiplier;
		cAmbientMultiplier_Start= cAmbientMultiplier;
	}
	if ( fFogLerp < 1.0f ) {
		fFogLerp += Time::deltaTime;
	}
	if ( fFogLerp > 1.0f ) {
		fFogLerp = 1.0f;
	}
	cFogMultiplier		= cFogMultiplier_Start.Lerp( cFogMultiplier_Target, fFogLerp );
	cFogOffset			= cFogOffset_Start.Lerp( cFogOffset_Target, fFogLerp );
	fFogEndMultiplier	= Math.Lerp( fFogLerp, fFogEndMultiplier_Start, fFogEndMultiplier_Target );
	cAmbientMultiplier	= cAmbientMultiplier_Start.Lerp( cAmbientMultiplier_Target, fFogLerp );
}

// Update fog effects (overridable)
void CEnvironmentEffects::UpdateFogEffects ( void )
{
	// Do Biome-based fog effects (also what about seasons?)
	Renderer::Settings.fogColor = (Renderer::Settings.fogColor * cFogMultiplier) + cFogOffset;
	Renderer::Settings.fogEnd *= fFogEndMultiplier;
	Renderer::Settings.fogStart = std::max<Real>( std::min<Real>( 10.0f, Renderer::Settings.fogEnd*0.1f ), Renderer::Settings.fogEnd / 30.0f );

	// Underwater camera effect
	if ( bCameraUnderwater )
	{
		Renderer::Settings.fogStart = -10.0f;
		Renderer::Settings.fogEnd = 30.0f;
		Renderer::Settings.fogColor.red *= 0.6f;
		Renderer::Settings.fogColor.green *= 0.7f;
		Renderer::Settings.fogColor.blue *= 0.75f;
	}

	// Todo: move to own segment
	Renderer::Settings.ambientColor = (Renderer::Settings.ambientColor * cAmbientMultiplier);

}

// Initialize the particle effects
void CEnvironmentEffects::InitializeParticleEffects ( void )
{
	pBiomeParticles = new CExtendableGameObject ();
	{
		CParticleEmitter* biome_emitter = new CParticleEmitter ( );
		biome_emitter->LoadFromFile( ".res\\particlesystems\\env\\b_grassland.pcf" );
		biome_emitter->vEmitterSize = Vector3d( 16.0f,16.0f,8.0f );
		biome_emitter->transform.SetParent( &(pBiomeParticles->transform) );
		//biome_emitter->RemoveReference();
		pBiomeEmitter = biome_emitter;
		pBiomeEmitter->SetActive( false );

		CParticleUpdater* biome_updater = new CParticleUpdater ( biome_emitter );
		biome_updater->AddModifier( new CParticleMod_Windmotion );
		//biome_updater->RemoveReference();

		CParticleRenderer* biome_renderer = new CParticleRenderer ( biome_emitter );
		glMaterial* mat_defglow = new glMaterial ();
		mat_defglow->passinfo.push_back( glPass() );
		mat_defglow->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		mat_defglow->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		mat_defglow->m_diffuse = Color( 1.0f,1,1 );
		mat_defglow->setTexture( 0, new CTexture(".res/textures/particles/pollen.png") );
		mat_defglow->passinfo[0].m_blend_mode = Renderer::BM_ADD;
		mat_defglow->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
		biome_renderer->SetMaterial( mat_defglow );

		pBiomeParticles->AddComponent( biome_emitter );
		pBiomeParticles->AddComponent( biome_updater );
		pBiomeParticles->AddComponent( biome_renderer );
	}

	pUnderwaterParticles = new CExtendableGameObject ();
	{
		CParticleEmitter* underwater_emitter = new CParticleEmitter ();
		underwater_emitter->LoadFromFile( ".res\\particlesystems\\env\\underwater.pcf" );
		underwater_emitter->vEmitterSize = Vector3d( 8.0f,8.0f,6.0f );
		underwater_emitter->transform.SetParent( &(pUnderwaterParticles->transform) );
		//underwater_emitter->RemoveReference();
		pUnderwaterEmitter = underwater_emitter;

		CParticleUpdater* underwater_updater = new CParticleUpdater ( underwater_emitter );
		//underwater_updater->AddModifier( new CParticleMod_Watermotion );
		//underwater_updater->RemoveReference();

		CParticleRenderer* underwater_renderer = new CParticleRenderer ( underwater_emitter );
		glMaterial* mat_defmuck = new glMaterial ();
		mat_defmuck->passinfo.push_back( glPass() );
		mat_defmuck->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
		mat_defmuck->m_diffuse = Color( 1.0f,1,1 );
		mat_defmuck->setTexture( 0, new CTexture(".res/textures/particles/fluxflame1.png") );
		mat_defmuck->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
		//mat_defmuck->iBlendMode = glMaterial::BM_ADD;
		underwater_renderer->SetMaterial( mat_defmuck );
		pUnderwaterRenderer = underwater_renderer;

		pUnderwaterParticles->AddComponent( underwater_emitter );
		pUnderwaterParticles->AddComponent( underwater_updater );
		pUnderwaterParticles->AddComponent( underwater_renderer );
	}
	pUnderwaterEmitter->SetActive( false );

	//pDustParticles = new CExtendableGameObject ();
	pDustParticles = new CParticleSystem( ".res/particlesystems/env/a_desert.pcf" );
	{
		pDustEmitter = ((CParticleSystem*)(pDustParticles))->GetEmitter();
		/*CParticleEmitter* dust_emitter = new CParticleEmitter ();
		dust_emitter->LoadFromFile( ".res\\particlesystems\\env\\a_desert.pcf" );
		dust_emitter->vEmitterSize = Vector3d( 2.0f, 2.0f, 2.0f );
		dust_emitter->transform.SetParent( &(pDustParticles->transform) );
		dust_emitter->RemoveReference();
		pDustEmitter = dust_emitter;

		CParticleUpdater* dust_updater = new CParticleUpdater ( dust_emitter );
		dust_updater->AddModifier( new CParticleMod_Windmotion(9.0f) );
		dust_updater->RemoveReference();

		CParticleRenderer* dust_renderer = new CParticleRenderer ( dust_emitter );
		glMaterial* mat_dust = new glMaterial;
		mat_dust->useLighting = false;
		mat_dust->isTransparent = true;
		mat_dust->useAlphaTest = false;
		mat_dust->useDepthMask = false;
		mat_dust->diffuse = Color( 1.0f,1,1 );
		mat_dust->loadTexture( ".res\\textures\\particles\\dust01.png" );
		dust_renderer->SetMaterial( mat_dust );
		
		pDustParticles->AddComponent( dust_emitter );
		pDustParticles->AddComponent( dust_updater );
		pDustParticles->AddComponent( dust_renderer );*/

		fDustCooldown = 0;
		fDustTimer = 0;
	}
	((CParticleSystem*)pDustParticles)->enabled = false;

	pWeatherParticles = new CExtendableGameObject ();
	{
		CParticleEmitter* weather_emitter = new CParticleEmitter ();
		weather_emitter->LoadFromFile( ".res/particlesystems/env/w_rain.pcf" );
		weather_emitter->vEmitterSize = Vector3d( 19.0f,19.0f,4.0f );
		weather_emitter->transform.SetParent( &(pWeatherParticles->transform) );
		//weather_emitter->RemoveReference();
		pWeatherEmitter = weather_emitter;
		pWeatherEmitter->SetActive( false );

		CParticleUpdater* weather_updater = new CParticleUpdater ( weather_emitter );
		weather_updater->AddModifier( new CParticleMod_Windmotion(2.0f) );
		//weather_updater->RemoveReference();

		CParticleRenderer* weather_renderer = new CParticleRenderer ( weather_emitter );
		glMaterial* mat_defweather = new glMaterial ();
		mat_defweather->passinfo.push_back( glPass() );
		mat_defweather->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		mat_defweather->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		mat_defweather->m_diffuse = Color( 1.0f,1,1 );
		mat_defweather->setTexture( 0, new CTexture(".res/textures/particles/pollen.png") );
		mat_defweather->passinfo[0].m_blend_mode = Renderer::BM_NORMAL;
		mat_defweather->passinfo[0].shader = new glShader( ".res/shaders/particles/colorBlended.glsl" );
		weather_renderer->SetMaterial( mat_defweather );
		weather_renderer->iRenderMethod = CParticleRenderer::P_STRETCHED_BILLBOARD;
		weather_renderer->fR_SpeedScale = 0.027f;

		pWeatherParticles->AddComponent( weather_emitter );
		pWeatherParticles->AddComponent( weather_updater );
		pWeatherParticles->AddComponent( weather_renderer );
	}
}
// Update the particle effects
void CEnvironmentEffects::UpdateParticleEffects ( void )
{
	// Change particle system based on the biome
	int nextBiome = iNextBiomeType;
	if ( nextBiome != iBiomeType )
	{
		if ((( nextBiome == Terrain::BIO_SAVANNA )||( nextBiome == Terrain::BIO_GRASSLAND ))
			&&(( iBiomeType != Terrain::BIO_SAVANNA )&&( iBiomeType != Terrain::BIO_GRASSLAND )))
		{
			pBiomeEmitter->LoadFromFile( ".res\\particlesystems\\env\\b_grassland.pcf" );
			pBiomeEmitter->SetActive( true );
		}
		else if ((( nextBiome == Terrain::BIO_SWAMP )||( nextBiome == Terrain::BIO_RAINFOREST )||( nextBiome == Terrain::BIO_OUTLANDS )||( nextBiome == Terrain::BIO_WET_OUTLANDS ))
			&&(( iBiomeType != Terrain::BIO_SWAMP )&&( iBiomeType != Terrain::BIO_RAINFOREST )&&( iBiomeType != Terrain::BIO_OUTLANDS )&&( iBiomeType != Terrain::BIO_WET_OUTLANDS )))
		{
			pBiomeEmitter->LoadFromFile( ".res\\particlesystems\\env\\b_jungle.pcf" );
			pBiomeEmitter->SetActive( true );
		}
		else
		{
			pBiomeEmitter->SetActive( false );
		}
	}

	// Move the biome emitter if it's active
	if ( pBiomeEmitter->GetActive() ) {
		pBiomeParticles->transform.position = vCameraPosition + (CCamera::activeCamera->transform.Forward()*16.0f) + Vector3d( 0,0,4 );
	}

	// Underwater particle effects
	if ( bCameraUnderwater )
	{
		pUnderwaterParticles->transform.position = vCameraPosition + (CCamera::activeCamera->transform.Forward()*6.0f);
		pUnderwaterParticles->active = true;
		pUnderwaterEmitter->SetActive( true );
		pUnderwaterRenderer->SetVisible( true );
	}
	else
	{
		pUnderwaterParticles->active = false;
		pUnderwaterEmitter->SetActive( false );
		pUnderwaterRenderer->SetVisible( false );
	}

	// Time the dust particles
	if ( fDustTimer > 0.0f ) {
		fDustTimer -= Time::deltaTime*3.0f;
	}
	else {
		fDustTimer = Random.Range( 0.7f,2.2f );
		fDustCooldown = Random.Range( 0.9f,3.2f );
		fDustOffset = Random.Range( 0,360 );
	}
	fDustCooldown -= Time::deltaTime;
	// Move the dust emitter if active
	if ( fDustTimer < 1.0f )
		pDustEmitter->transform.position = vCameraPosition + (CCamera::activeCamera->transform.Forward()*16.0f) + Rotator(0,0,(Time::currentTime-fDustCooldown)*40.0f+fDustOffset)*Vector3d( 30+40*sin( Time::currentTime ),0,2-20*sin( Time::currentTime ) );
	// Activate emitter at certain times
	if (( nextBiome == Terrain::BIO_DESERT )||( nextBiome == Terrain::BIO_COLD_DESERT )||( nextBiome == Terrain::BIO_GRASS_DESERT )) {
		//pDustEmitter->active = ( (fDustCooldown > 0.2f) && (fDustCooldown < 1.8f) );
		pDustEmitter->SetActive ( (fDustCooldown > -0.2f) && (fDustCooldown < 2.3f) );
		((CParticleSystem*)pDustParticles)->enabled = true;
	}
	else {
		pDustEmitter->SetActive( false );
	}

	// Change particle system based on the weather
	int nextWeather = iNextWeatherType;
	if ( (nextWeather != iWeatherType) || bCameraIndoorStateChanged )
	{
		//cout << "bCameraIndoors: " << std::boolalpha << bCameraIndoors << endl;
		if ( bCameraIndoors )
		{
			if ( pHeavyWeatherEffect ) {
				//delete_safe( pHeavyWeatherEffect );
				pHeavyWeatherEffect->SetInside( true );
			}
			pWeatherEmitter->SetActive( false );
		}
		else
		{
			if ((( nextWeather == Terrain::WTH_LIGHT_RAIN )||( nextWeather == Terrain::WTH_HEAVY_RAIN ))
				&&(( iWeatherType != Terrain::WTH_LIGHT_RAIN )&&( iWeatherType != Terrain::WTH_HEAVY_RAIN )))
			{
				pWeatherEmitter->LoadFromFile( ".res/particlesystems/env/w_rain.pcf" );
				pWeatherEmitter->SetActive( true );
			}
			else if ((( nextWeather == Terrain::WTH_LIGHT_RAIN_THUNDER )||( nextWeather == Terrain::WTH_HEAVY_RAIN_THUNDER ))
				&&(( iWeatherType != Terrain::WTH_LIGHT_RAIN_THUNDER )&&( iWeatherType != Terrain::WTH_HEAVY_RAIN_THUNDER )))
			{
				pWeatherEmitter->LoadFromFile( ".res/particlesystems/env/w_rain.pcf" );
				pWeatherEmitter->SetActive( true );
			}
			else if ((( nextWeather == Terrain::WTH_LIGHT_SNOW )||( nextWeather == Terrain::WTH_HEAVY_SNOW )||( nextWeather == Terrain::WTH_STORM_SNOW ))
				&&(( iWeatherType != Terrain::WTH_LIGHT_SNOW )&&( iWeatherType != Terrain::WTH_HEAVY_SNOW )&&( iWeatherType != Terrain::WTH_STORM_SNOW )))
			{
				pWeatherEmitter->LoadFromFile( ".res/particlesystems/env/w_snow.pcf" );
				pWeatherEmitter->SetActive( true );
			}
			else if (( nextWeather == Terrain::WTH_NONE )&&( iWeatherType != Terrain::WTH_NONE ))
			{
				pWeatherEmitter->SetActive( false );
			}
			else if ( nextWeather != Terrain::WTH_NONE )
			{
				pWeatherEmitter->SetActive( false );
			}

			if ( nextWeather == Terrain::WTH_HEAVY_RAIN || nextWeather == Terrain::WTH_HEAVY_RAIN_THUNDER || nextWeather == Terrain::WTH_HEAVY_SNOW || nextWeather == Terrain::WTH_STORM_SNOW ) {
				if ( !pHeavyWeatherEffect ) {
					pHeavyWeatherEffect = new EnvHeavyWeather;
				}
				pHeavyWeatherEffect->SetWeather( nextWeather );
				pHeavyWeatherEffect->SetInside( false );
			}
			else {
				/*if ( pHeavyWeatherEffect ) {
					delete_safe( pHeavyWeatherEffect );
				}*/
				if ( pHeavyWeatherEffect ) {
					pHeavyWeatherEffect->SetWeather( Terrain::WTH_NONE );
				}
			}
		}
	}

	// Move the weather emitter if it's active
	if ( pWeatherEmitter->GetActive() ) {
		pWeatherParticles->transform.position = vCameraPosition + (CCamera::activeCamera->transform.Forward()*14.0f) + Vector3d( 0,0,9 ); 
	}
}
// Free the particle effects
void CEnvironmentEffects::FreeParticleEffects ( void )
{
	delete pBiomeParticles;
	delete pUnderwaterParticles;
	delete pDustParticles;
	delete pWeatherParticles;
}

// Initialize the sound effects
void CEnvironmentEffects::InitializeSoundEffects ( void )
{
	sndBeach = Audio.playSound( "Env.Beach.Shoreline1" );
	sndBeach->deleteWhenDone = false;
	sndBeach->SetGain( 0 );
	sndWind	 = Audio.playSound( "Env.Ambient.Wind2" );
	sndWind->deleteWhenDone = false;
	sndWind->SetGain( 0 );
	sndRain	 = Audio.playSound( "Env.HeavyRain" );
	sndRain->deleteWhenDone = false;
	sndRain->SetGain( 0 );

	fBeachGain = 0;
	fWindGain = 0;
	fWindPitch = 0;
	fRainGain = 0;
}
// Update sound effects
void CEnvironmentEffects::UpdateSoundEffects ( void )
{
	ftype fBeachGain_Target, fWindGain_Target, fWindPitch_Target, fRainGain_Target;

	// First update the terrain-based sound effects
	char iTerrainType;
	iTerrainType = Zones.GetTerrainTypeAt( vCameraPosition );
	if ( iTerrainType == Terrain::TER_ISLANDS || iTerrainType == Terrain::TER_OCEAN || iTerrainType == Terrain::TER_SPIRES ) {
		fBeachGain_Target = 0.5f;
	}
	else {
		fBeachGain_Target = 0.0f;
	}

	fBeachGain += (fBeachGain_Target-fBeachGain) * Time::TrainerFactor( 0.01f );
	sndBeach->SetGain( fBeachGain );

	sndBeach->position = vCameraPosition;

	// Update the wind-based sound effect
	Vector3d windDir = WindMotion.GetFlowField( vCameraPosition );
	fWindGain_Target = windDir.magnitude() * 4.0f - 0.2f;
	if ( fWindGain_Target > 0.8f ) {
		fWindGain_Target = 0.8f;
	}
	if ( fWindGain_Target < 0.0f ) {
		fWindGain_Target = 0.0f;
	}
	fWindPitch_Target = windDir.magnitude() * 3.0f + 0.2f;
	if ( fWindPitch_Target > 1.4f ) {
		fWindPitch_Target = 1.4f;
	}
	if ( fWindPitch_Target < 0.5f ) {
		fWindPitch_Target = 0.5f;
	}

	fWindGain += (fWindGain_Target-fWindGain) * Time::TrainerFactor( 0.05f );
	sndWind->SetGain( fWindGain );

	fWindPitch += (fWindPitch_Target-fWindPitch) * Time::TrainerFactor( 0.008f );
	sndWind->SetPitch( fWindPitch );

	sndWind->position = vCameraPosition;

	// Update the rain sound effect
	if ( iWeatherType == Terrain::WTH_HEAVY_RAIN || iWeatherType == Terrain::WTH_HEAVY_RAIN_THUNDER ) {
		fRainGain_Target = 0.8f;
	}
	else if ( iWeatherType == Terrain::WTH_LIGHT_RAIN || iWeatherType == Terrain::WTH_LIGHT_RAIN_THUNDER ) 
	{
		fRainGain_Target = 0.2f;
	}
	else {
		fRainGain_Target = 0.0f;
	}

	fRainGain += (fRainGain_Target-fRainGain) * Time::TrainerFactor( 0.04f );
	sndRain->SetGain( fRainGain );

	sndRain->position = vCameraPosition;

}
// Free the sound effects
void CEnvironmentEffects::FreeSoundEffects ( void )
{
	delete sndBeach;
	delete sndWind;
	delete sndRain;
}


// Update color filtering
void CEnvironmentEffects::UpdateColorFilter ( void )
{
	if ( pColorFilter == NULL )
	{
		pColorFilter = (CColorFilterShader*)((CRendererHolder*)CGameState::Active()->FindFirstObjectWithName( "Color Filter Shader Holder" ))->GetRenderer();
		return;
	}

	// These are the defaults.
	/*
	m_midtone_blend		= Color( 1,1,1,-0.05 );
	m_highlight_blend	= Color( 1,1,1,0.05 ); 
	m_shadow_blend		= Color( 0.98,0.98,1.02,0.08 );
	*/

	// Set the colors first based on the base temperature

	// When it's cold, shadows will be more purple and saturated. Everything else is more desaturated.
	// When it's warm, shadows are colder, highlights are warmer, and midtones are more saturated.
	//pColorFilter->m_highlight_blend ;
	//pColorFilter->m_midtone_blend.alpha		= -0.05;
	//pColorFilter->m_shadow_blend.alpha		= 0.08;

	fTemperatureSmoother += (fTemperature-fTemperatureSmoother) * Time::TrainerFactor( 0.02f );

	if ( iBiomeType == Terrain::BIO_S_TUNDRA || iWeatherType == Terrain::WTH_STORM_SNOW ) {
		fTemperatureSmoother += (-2-fTemperatureSmoother) * Time::TrainerFactor( 0.05f );
	}

	ftype temperatureOffset = (fTemperatureSmoother-1)*2;
	//temperatureOffset = cub(temperatureOffset);

	/*pColorFilter->m_shadow_blend = Color(
		0.98 - std::min<ftype>(temperatureOffset*0.06f,0),0.98,1.02 + std::max<ftype>(temperatureOffset*0.2f,0),
		0.08 - std::min<ftype>(temperatureOffset*0.3f,0)	// saturation
		);
	pColorFilter->m_midtone_blend = Color(
		1,1,1,
		-0.05 - std::min<ftype>(temperatureOffset*0.2f,0.1f) // saturation
		);
	pColorFilter->m_highlight_blend = Color(
		1 + temperatureOffset*0.1f,1 + temperatureOffset*0.05,1,
		0.05 - std::min<ftype>( temperatureOffset*0.1f,0.05f )//saturation
		);*/

	Color m_shadow_froz ( 0.8,0.8,1.3,1.0 );
	Color m_midtone_froz ( 0.9,0.9,1.1,-0.3 );
	Color m_highlight_froz ( 1.0,0.9,1.2,0.7 );

	Color m_shadow_cold ( 0.9,0.9,1.2,0.3 );
	Color m_midtone_cold ( 1.0,1.0,1.05,-0.2 );
	Color m_highlight_cold ( 1.0,0.9,1.2,0.6 );

	Color m_shadow_neutral ( 0.98,0.98,1.02,0.08 );
	Color m_midtone_neutral ( 1,1,1,-0.05 );
	Color m_highlight_neutral ( 1,1,1,0.05 ); 

	/*Color m_shadow_warm ( 1.0,0.9,1.0,-0.3 );
	Color m_midtone_warm ( 0.9,0.9,0.9,0.1 );
	Color m_highlight_warm ( 1.1,1.0,0.9,-0.2 );*/

	Color m_shadow_warm ( 1.0,0.9,1.0,-0.4 );
	Color m_midtone_warm ( 0.9,0.9,0.9,-0.2 );
	Color m_highlight_warm ( 1.1,1.0,0.9,-0.3 );

	if ( temperatureOffset < -1 ) {
		pColorFilter->m_shadow_blend = Color::Lerp( m_shadow_cold, m_shadow_froz, -1-temperatureOffset );
		pColorFilter->m_midtone_blend = Color::Lerp( m_midtone_cold, m_midtone_froz, -1-temperatureOffset );
		pColorFilter->m_highlight_blend = Color::Lerp( m_highlight_cold, m_highlight_froz, -1-temperatureOffset );
	}
	else if ( temperatureOffset < 0 ) {
		pColorFilter->m_shadow_blend = Color::Lerp( m_shadow_neutral, m_shadow_cold, -temperatureOffset );
		pColorFilter->m_midtone_blend = Color::Lerp( m_midtone_neutral, m_midtone_cold, -temperatureOffset );
		pColorFilter->m_highlight_blend = Color::Lerp( m_highlight_neutral, m_highlight_cold, -temperatureOffset );
	}
	else {
		pColorFilter->m_shadow_blend = Color::Lerp( m_shadow_neutral, m_shadow_warm, temperatureOffset );
		pColorFilter->m_midtone_blend = Color::Lerp( m_midtone_neutral, m_midtone_warm, temperatureOffset );
		pColorFilter->m_highlight_blend = Color::Lerp( m_highlight_neutral, m_highlight_warm, temperatureOffset );
	}
}