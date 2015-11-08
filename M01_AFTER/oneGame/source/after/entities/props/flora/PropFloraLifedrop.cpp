
#include "PropFloraLifedrop.h"
#include "after/entities/item/props/flora/ItemFloraLifedrop.h"

#include "core/math/random/Random.h"

// Constructor (on terrain)
PropFloraLifedrop::PropFloraLifedrop ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo )
{
	m_model = new CModel( string(".res/models/flora/lifedrop_fern.FBX") );
}

// Destructor
PropFloraLifedrop::~PropFloraLifedrop ( void )
{
	;
}

// Terrain placement. Override the set scale and rotation
void PropFloraLifedrop::OnTerrainPlacement ( void )
{
	transform.scale = Vector3d( 0.7f,0.7f,0.7f ) * Random.Range( 0.8f, 1.4f );
	transform.rotation = Vector3d( Random.Range(-15.0f,15.0f),Random.Range(-15.0f,15.0f),Random.Range(0,360.0f) );
}

// Lookats create some outlines
void	PropFloraLifedrop::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		m_modelGlow->SetVisible( true );
		m_modelGlow->m_color = Color( 1.0f,1,1 );
		m_modelGlow->m_width = 0.08f;
	}
}
void	PropFloraLifedrop::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		m_modelGlow->SetVisible( false );
	}
}
// Usage creates drops
void	PropFloraLifedrop::OnInteract ( CActor* interactingActor )
{
	ItemFloraLifedrop* pNewItem = new ItemFloraLifedrop();
	pNewItem->transform.Get( transform );
	pNewItem->transform.position.z += 0.5f;

	DeleteObject( this );
}


// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void PropFloraLifedrop::OnPunched ( RaycastHit const& hitInfo )
{
	ItemFloraLifedrop* pNewItem = new ItemFloraLifedrop();
	pNewItem->transform.Get( transform );
	pNewItem->transform.position.z += 0.5f;

	DeleteObject( this );
}