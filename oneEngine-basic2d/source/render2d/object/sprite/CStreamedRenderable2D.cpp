
#include "render2d/object/sprite/CStreamedRenderable2D.h"

CStreamedRenderable2D::CStreamedRenderable2D ( void )
	: CRenderable2D()
{
	;
}
CStreamedRenderable2D::~CStreamedRenderable2D ( void )
{
	;
}

// Return access to model data
CModelData* CStreamedRenderable2D::GetModelData ( void )
{
	return &m_modeldata;
}

// Push the current stuff in model data to GPU.
void CStreamedRenderable2D::StreamLockModelData ( void )
{
	PushModeldata();
}