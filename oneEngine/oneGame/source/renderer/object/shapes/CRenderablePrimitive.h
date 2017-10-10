//===============================================================================================//
//
//		Renderable Primitive
//
// The renderable primitive is not primitive, but renders a given mode definition.
// To render using this class, derive from this class, create a model, then call PushModelData.
// ``The 2D equivalent is CRenderable2D.``
//
//===============================================================================================//

#ifndef C_RENDERABLE_PRIMITIVE_
#define C_RENDERABLE_PRIMITIVE_

#include "core/types/ModelData.h"
#include "core/math/Vector3d.h"
#include "../CRenderableObject.h"
#include "renderer/material/RrMaterial.h"

class CRenderablePrimitive : public CRenderableObject
{
public:
	RENDER_API				CRenderablePrimitive ( void );
	RENDER_API				~CRenderablePrimitive ( void );

	//		PreRender()
	// Push the uniform properties
	bool					PreRender ( void ) override;

	//		Render()
	// Render the model using the 2D engine's style
	bool					Render ( const char pass ) override;

protected:

	//		PushModelData()
	// Takes the information inside of m_modeldata and pushes it to the GPU so that it may be rendered.
	RENDER_API void			PushModeldata ( void );

	//	Mesh information
	arModelData				m_modeldata;
	//	GPU information
	uint					m_buffer_verts;
	uint					m_buffer_tris;
};


#endif//C_RENDERABLE_PRIMITIVE_