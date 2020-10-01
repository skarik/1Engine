//===============================================================================================//
//
//	class CStreamedRenderable3D
//
// Common class for rendering in the 3D system
// Variation of CEditableRenderable3D in that all meshes are delayed by one frame, which
// allows time for uploading a mesh
//
//===============================================================================================//

#ifndef RENDER_C_STREAMED_RENDERABLE_3D_H_
#define RENDER_C_STREAMED_RENDERABLE_3D_H_

#include "renderer/object/CRenderable3D.h"

class CStreamedRenderable3D : public CRenderable3D
{
public:
	RENDER_API explicit		CStreamedRenderable3D ( void );
	RENDER_API virtual		~CStreamedRenderable3D ();

	//	GetModelData ( ) 
	// Return access to model data
	RENDER_API arModelData*	GetModelData ( void );

	//	StreamLockModelData ( )
	// Push the current stuff in model data to GPU.
	RENDER_API void			StreamLockModelData ( void );

	//	PreRender()
	// Push the uniform properties
	RENDER_API bool			PreRender ( rrCameraPass* cameraPass ) override;

	//	Render()
	// Render the model using the 2D engine's style
	RENDER_API bool			Render ( const rrRenderParams* params ) override;

	//	EndRender()
	// Called after the frame is being rendered. Swaps the currently used buffers
	RENDER_API bool			EndRender ( void ) override;

protected:
	//	State
	bool					m_wants_swap;
	//	Mesh information
	/*uint					m_model_tricount;
	uint					m_next_model_tricount;
	//	GPU information
	uint					m_next_buffer_verts;
	uint					m_next_buffer_tris;*/

	//	Mesh information
	uint					m_model_indexcount;
	uint					m_next_model_indexcount;
	//	GPU information
	rrMeshBuffer			m_meshBufferAux;
	rrMeshBuffer*			m_currentMeshBuffer;

	//	Render callbacks
	rrMaterialRenderFunction
							m_postMaterialCb = NULL;
};

#endif//RENDER_C_STREAMED_RENDERABLE_3D_H_