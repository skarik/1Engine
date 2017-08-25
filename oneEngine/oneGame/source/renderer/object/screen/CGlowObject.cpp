
#include "CGlowObject.h"
#include "renderer/logic/model/CModel.h"

CGlowObject::CGlowObject ( const CModel* n_model )
	: CRenderableObject()
{
	model = (CModel*)n_model;

	//throw core::NotYetImplementedException();
}

bool CGlowObject::Render ( const char pass )
{
	throw core::NotYetImplementedException();
	return true;
}