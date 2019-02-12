
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

	id = RrRenderer::Active->AddLO( this );
	active = true;
}
// ==Destructor
//  removes render object from the list of RO in RrRenderer
RrLogicObject::~RrLogicObject ( void )
{
	active = false;
	RrRenderer::Active->RemoveLO( id );
}