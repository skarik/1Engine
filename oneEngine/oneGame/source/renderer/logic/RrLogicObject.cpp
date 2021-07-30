
// == Includes ==
#include "renderer/state/RrRenderer.h"
#include "RrLogicObject.h"


// ==Constructor
//  adds render object to the list of RO in RrRenderer
//  sets visibility to true
RrLogicObject::RrLogicObject ( void )
{
	/*transform.owner = this;
	transform.ownerType = Transform::kOwnerTypeRendererLogic;
	transform.name = "Transform(RenderLogic)";*/
	//transform.name = this->name; // TODO: ADD NAME

	//id = RrRenderer::Active->AddLO( this );
	RrRenderer::Listings::AddToUnsorted(this, id);
	active = true;
}
// ==Destructor
//  removes render object from the list of RO in RrRenderer
RrLogicObject::~RrLogicObject ( void )
{
	active = false;
	//RrRenderer::Active->RemoveLO( id );
	RrRenderer::Listings::GetWorld(id)->RemoveLogic(this);
}

void RrLogicObject::AddToWorld ( RrWorld* world )
{
	ARCORE_ASSERT_MSG(world->world_index != UINT32_MAX, "Invalid world. Was the world added to a renderer first?");
	ARCORE_ASSERT(id.world_index == rrId::kWorldInvalid);
	RrRenderer::Listings::RemoveFromUnsorted(this, id);
	id = world->AddLogic(this);
}