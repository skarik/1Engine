
// == Includes ==
#include "renderer/state/RrRenderer.h"
#include "CLogicObject.h"


// ==Constructor
//  adds render object to the list of RO in RrRenderer
//  sets visibility to true
CLogicObject::CLogicObject ( void )
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
CLogicObject::~CLogicObject ( void )
{
	active = false;
	RrRenderer::Active->RemoveLO( id );
}