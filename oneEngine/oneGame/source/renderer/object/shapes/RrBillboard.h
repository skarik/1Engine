#ifndef RENDERER_RENDERABLE_SHAPE_BILLBOARD_H_
#define RENDERER_RENDERABLE_SHAPE_BILLBOARD_H_

#include "renderer/object/CRenderableObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"
#include <vector>

class RrBillboard : public CRenderableObject
{
public:
	RENDER_API explicit		RrBillboard ( void );
	RENDER_API				~RrBillboard ( void );

	//		PreRender()
	// Updates rotation and pushes the uniform properties. Called per camera.
	RENDER_API bool			PreRender ( rrCameraPass* cameraPass ) override;
	//		Render()
	// Renders the billboard.
	RENDER_API bool			Render ( const rrRenderParams* params ) override;

public:
	// If billboard should point at camera (true) or match the camera orientation (false).
	// Defaults to match camera orientation (false).
	bool				pointAtCamera;
	// Width (and height) of the billboard.
	float				width;

protected:
	static void				BuildMeshBuffer ( void );
	static rrMeshBuffer	m_MeshBuffer;
};

#endif//RENDERER_RENDERABLE_SHAPE_BILLBOARD_H_