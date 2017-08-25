//===============================================================================================//
//	class CEditableRenderable2D
//
// Common class for rendering in the 2D system, which takes care of the deferred shading
//
//
//===============================================================================================//

#ifndef _RENDER2D_C_EDITABLE_RENDERABLE_2D_H_
#define _RENDER2D_C_EDITABLE_RENDERABLE_2D_H_

#include "render2d/object/CRenderable2D.h"

class CEditableRenderable2D : public CRenderable2D
{
public:
	RENDER2D_API explicit	CEditableRenderable2D ( void );
	RENDER2D_API virtual	~CEditableRenderable2D ();

	//		GetModelData ( ) 
	// Return access to model data
	RENDER2D_API arModelData* GetModelData ( void );

	//		StreamLockModelData ( )
	// Push the current stuff in model data to GPU.
	RENDER2D_API void StreamLockModelData ( void );
};

#endif//_RENDER2D_C_STREAMED_RENDERABLE_2D_H_