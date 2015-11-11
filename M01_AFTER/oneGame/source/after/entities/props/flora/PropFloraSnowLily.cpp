
#include "PropFloraSnowLily.h"
#include "after/entities/item/props/flora/ItemFloraSnowLily.h"

#include "core/math/random/Random.h"

// Constructor (on terrain)
PropFloraSnowLily::PropFloraSnowLily ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo )
{
	m_model = new CModel( string("models/flora/snowdrop_lily.FBX") );
}

// Destructor
PropFloraSnowLily::~PropFloraSnowLily ( void )
{
	;
}

// Terrain placement. Override the set scale and rotation
void PropFloraSnowLily::OnTerrainPlacement ( void )
{
	transform.scale = Vector3d( 1.5f,1.5f,1.5f ) * Random.Range( 0.9f, 1.1f );
	transform.rotation = Vector3d( Random.Range(-15.0f,15.0f),Random.Range(-15.0f,15.0f),Random.Range(0,360.0f) );
}

// Lookats create some outlines
void	PropFloraSnowLily::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		m_modelGlow->SetVisible( true );
		m_modelGlow->m_color = Color( 1.0f,1,1 );
		m_modelGlow->m_width = 0.08f;
	}
}
void	PropFloraSnowLily::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		m_modelGlow->SetVisible( false );
	}
}
// Drop item on use
void PropFloraSnowLily::OnInteract	( CActor* interactingActor )
{
	ItemFloraSnowLily* pNewItem = new ItemFloraSnowLily();
	pNewItem->transform.Get( transform );

	DeleteObject( this );
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void PropFloraSnowLily::OnPunched ( RaycastHit const& hitInfo )
{
	ItemFloraSnowLily* pNewItem = new ItemFloraSnowLily();
	pNewItem->transform.Get( transform );

	DeleteObject( this );
}