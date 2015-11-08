//===============================================================================================//
//	class CRenderable2D
//
// Common class for rendering in the 2D system, which takes care of the deferred shading
//
//
//===============================================================================================//


#ifndef _RENDER2D_C_RENDERABLE_2D_H_
#define _RENDER2D_C_RENDERABLE_2D_H_

#include "renderer/object/CRenderableObject.h"

class CRenderable2D : public CRenderableObject
{
public:
	RENDER2D_API explicit	CRenderable2D ( void );
	RENDER2D_API virtual	~CRenderable2D ();
};

#endif//_RENDER2D_C_RENDERABLE_2D_H_