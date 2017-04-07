
#include "render2d/object/sprite/CEditableRenderable2D.h"

CEditableRenderable2D::CEditableRenderable2D ( void )
	: CRenderable2D()
{
	;
}
CEditableRenderable2D::~CEditableRenderable2D ( void )
{
	;
}

// Return access to model data
CModelData* CEditableRenderable2D::GetModelData ( void )
{
	return &m_modeldata;
}

// Push the current stuff in model data to GPU.
void CEditableRenderable2D::StreamLockModelData ( void )
{
	PushModeldata();
}