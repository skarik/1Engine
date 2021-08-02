#ifndef RENDERER_RENDERABLE_SHAPE_CUBE_H_
#define RENDERER_RENDERABLE_SHAPE_CUBE_H_

#include "core/types/ModelData.h"
#include "core/math/Vector3.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

class RrShapeCube : public RrRenderObject
{
public:
	RENDER_API explicit		RrShapeCube ( void );

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

#endif//RENDERER_RENDERABLE_SHAPE_CUBE_H_