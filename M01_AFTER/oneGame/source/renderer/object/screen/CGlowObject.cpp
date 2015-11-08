
#include "CGlowObject.h"
#include "renderer/logic/model/CModel.h"

CGlowObject::CGlowObject ( const CModel* n_model )
	: CRenderableObject()
{
	model = (CModel*)n_model;

	//throw Core::NotYetImplementedException();
}

bool CGlowObject::Render ( const char pass )
{
	throw Core::NotYetImplementedException();
	return true;
}