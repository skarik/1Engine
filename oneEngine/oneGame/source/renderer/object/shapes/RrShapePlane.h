#ifndef RENDERER_RENDERABLE_SHAPE_PLANE_H_
#define RENDERER_RENDERABLE_SHAPE_PLANE_H_

#include "renderer/object/CRenderable3D.h"

class RrShapePlane : public CRenderableObject
{
public:
	RENDER_API explicit		RrShapePlane ( void );

	//		PreRender()
	// Push the uniform properties.
	RENDER_API bool			PreRender ( rrCameraPass* cameraPass ) override;

	//		Render()
	// Renders the shape.
	RENDER_API bool			Render ( const rrRenderParams* params ) override;

protected:
	static void				BuildMeshBuffer ( void );
	static rrMeshBuffer	m_MeshBuffer;
};

#endif//RENDERER_RENDERABLE_SHAPE_PLANE_H_