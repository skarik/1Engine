
// == Includes ==
#include "renderer/state/CRenderState.h"
#include "CLogicObject.h"


// ==Constructor
//  adds render object to the list of RO in CRenderState
//  sets visibility to true
CLogicObject::CLogicObject ( void )
{
	/*transform.owner = this;
	transform.ownerType = Transform::kOwnerTypeRendererLogic;
	transform.name = "Transform(RenderLogic)";*/
	//transform.name = this->name; // TODO: ADD NAME

	id = CRenderState::Active->AddLO( this );
	active = true;
}
// ==Destructor
//  removes render object from the list of RO in CRenderState
CLogicObject::~CLogicObject ( void )
{
	active = false;
	CRenderState::Active->RemoveLO( id );
}