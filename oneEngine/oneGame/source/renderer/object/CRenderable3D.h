//===============================================================================================//
//
//		Renderable 3D
//
// A simple class that renders a given model definition.
// To render using this class, derive from this class, create a model, then call PushModelData.
// ``The 2D equivalent is CRenderable2D.``
//
//===============================================================================================//

#ifndef C_RENDERABLE_3D_
#define C_RENDERABLE_3D_

#include "core/types/ModelData.h"
#include "core/math/Vector3.h"
#include "renderer/object/RrRenderObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

class CRenderable3D : public RrRenderObject
{
public:
	RENDER_API				CRenderable3D ( void );
	RENDER_API				~CRenderable3D ( void );

	//		PreRender()
	// Push the uniform properties
	RENDER_API bool			PreRender ( rrCameraPass* cameraPass ) override;

	//		Render()
	// Render the model using the 2D engine's style
	RENDER_API bool			Render ( const rrRenderParams* params ) override;

protected:

	//		PushModelData()
	// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
	RENDER_API void			PushModeldata ( void );

	//	Mesh information
	arModelData				m_modeldata;
	//	GPU information
	rrMeshBuffer			m_meshBuffer;
};

#endif//C_RENDERABLE_PRIMITIVE_