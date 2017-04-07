//===============================================================================================//
//	class CStreamedRenderable2D
//
// Common class for rendering in the 2D system, which takes care of the deferred shading
// Variation of CEditableRenderable2D in that all meshes are delayed by one frame, which
// allows time for uploading a mesh
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

	//		Render()
	// Render the model using the 2D engine's style
	RENDER2D_API bool		Render ( const char pass ) override;
	
	//		EndRender()
	// Called after the frame is being rendered. Swaps the currently used buffers
	RENDER2D_API bool		EndRender ( void ) override;

protected:
	//	State
	bool					m_wants_swap;
	//	Mesh information
	uint					m_model_tricount;
	uint					m_next_model_tricount;
	//	GPU information
	uint					m_next_buffer_verts;
	uint					m_next_buffer_tris;
};

#endif//_RENDER2D_C_STREAMED_RENDERABLE_2D_H_