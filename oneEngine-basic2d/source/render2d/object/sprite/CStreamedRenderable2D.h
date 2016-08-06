//===============================================================================================//
//	class CStreamedRenderable2D
//
// Common class for rendering in the 2D system, which takes care of the deferred shading
//
//
//===============================================================================================//

#ifndef _RENDER2D_C_STREAMED_RENDERABLE_2D_H_
#define _RENDER2D_C_STREAMED_RENDERABLE_2D_H_

#include "render2d/object/CRenderable2D.h"

class CStreamedRenderable2D : public CRenderable2D
{
public:
	RENDER2D_API explicit	CStreamedRenderable2D ( void );
	RENDER2D_API virtual	~CStreamedRenderable2D ();

	//		GetModelData ( ) 
	// Return access to model data
	RENDER2D_API CModelData* GetModelData ( void );

	//		StreamLockModelData ( )
	// Push the current stuff in model data to GPU.
	RENDER2D_API void StreamLockModelData ( void );
};

#endif//_RENDER2D_C_STREAMED_RENDERABLE_2D_H_