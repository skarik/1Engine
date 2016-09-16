
#include "PropCampfire.h"
#include "after/entities/item/props/ItemCampfire.h"

#include "renderer/light/CLight.h"
#include "engine-common/entities/CParticleSystem.h"

#include "after/renderer/objects/hud/Plhud_TextNotifier.h"

#include "after/physics/water/Water.h"
#include "after/terrain/Zones.h"
#include "after/physics/wind/WindMotion.h"
#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/WeatherTypes.h"

// Constructor (on terrain)
PropCampfire::PropCampfire ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo ), mHitValue(0), bIsLit( false )
{
	m_model = new CModel( "models/props/campfire.FBX" );
	transform.scale = Vector3d( 1.4f,1.4f,1.4f );

	pLight = NULL;
	pFireSystem = NULL;
}
// Destructor
PropCampfire::~PropCampfire ( void )
{
	FreeEffects();
}

// Create and destroy fire effects
void PropCampfire::CreateEffects ( void )
{
	if ( !pLight ) {
		pLight = new CLight();
	}

	if ( !pFireSystem ) {
		pFireSystem = new CParticleSystem ( ".res/particlesystems/campfire.pcf" );
	}
}
void PropCampfire::FreeEffects ( void )
{
	delete_safe( pLight );
	delete_safe( pFireSystem );
}

// Update
void PropCampfire::Update ( void )
{
	if ( bIsLit ) {
		CreateEffects();
		pLight->diffuseColor = Color( 0.71f, 0.55f, 0.24f ) * random_range( 1.2f,1.3f );
		pLight->range = 48.0f;
		pLight->falloff = 1.5f;
		pLight->pass = 0.3f;

		pLight->drawHalo = true;

		pLight->transform.Get( transform );
		pLight->transform.position.z += 0.8f;

		pFireSystem->transform.position = transform.position + !Quaternion(-transform.rotation.getEulerAngles())*Vector3d( 0,0,-0.41f );
	}
	else {
		FreeEffects();
	}

	if ( mHitValue > 0 ) {
		mHitValue -= Time::deltaTime;
	}
	
	/*if ( this->mBlockInfo.pBlock->block == 0 ) {
		ItemCampfire* pNewItem = new ItemCampfire();
		pNewItem->transform.Get( transform );

		DeleteObject( this );
	}*/
}

// Interact turns on
void	PropCampfire::OnInteract ( CActor* interactingActor )
{
	if ( !bIsLit ) {
		bool canLight = true;

		// Check for underwater
		if ( WaterTester::Get()->PositionInside( transform.position+Vector3d( 0,0,0.7f ) ) ) {
			canLight = false;
			new Plhud_TextNotifier( "Can't light fire in water." );
		}
		// Check for permitting weather
		if ( canLight )
		{
			uchar weather = WindMotion.GetWeather(transform.position);
			switch ( weather ) {
				case Terrain::WTH_HEAVY_RAIN:
				case Terrain::WTH_HEAVY_RAIN_THUNDER:
				case Terrain::WTH_STORM_SNOW:
				case Terrain::WTH_ICE_STORM:
				case Terrain::WTH_DUST_STORM:
				case Terrain::WTH_FLUXX_STORM:
					canLight = false;
					new Plhud_TextNotifier( "Too stormy to light fire." );
					break;
			}
		}
		// Check for too windy
		if ( canLight )
		{
			if ( WindMotion.GetFlowFieldFast( transform.position ).magnitude() > 30 ) {
				canLight = false;
				new Plhud_TextNotifier( "Too windy to light fire." );
			}
		}

		// Finally light the gorramn fire
		if ( canLight ) {
			bIsLit = true;
		}
	}
}
// Lookats create some outlines
void	PropCampfire::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		m_modelGlow->SetVisible( true );
		m_modelGlow->m_color = Color( 1.0f,1,1 );
		m_modelGlow->m_width = 0.08f;
	}
}
void	PropCampfire::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		m_modelGlow->SetVisible( false );
	}
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void PropCampfire::OnPunched ( RaycastHit const& hitInfo )
{
	mHitValue += 2;
	if ( mHitValue > 4 )
	{
		ItemCampfire* pNewItem = new ItemCampfire();
		pNewItem->transform.Get( transform );

		DeleteObject( this );
	}
}

// Save data
uint64_t PropCampfire::GetUserdata ( void ) 
{
	if ( bIsLit ) {
		return 1;
	}
	else {
		return 0;
	}
}
// Load data
void PropCampfire::SetUserdata ( const uint64_t& input )
{
	if ( input ) {
		bIsLit = true;
	}
	else {
		bIsLit = false;
	}
}