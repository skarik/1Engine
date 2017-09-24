#ifndef RENDERER_OBJECT_IMMEDIAT_RR_SCOPED_MESH_RENDERER_H_
#define RENDERER_OBJECT_IMMEDIAT_RR_SCOPED_MESH_RENDERER_H_

#include "IRrRenderableStreamedMesh.h"

#include "renderer/utils/IrrMeshBuilder.h"
#include "renderer/object/CRenderableObject.h"

class RrScopedMeshRenderer : public IRrRenderableStreamedMesh
{
public:
	RENDER_API explicit		RrScopedMeshRenderer ( void );

	//	render() : Renders the given mesh builder.
	// This will not pass in mesh uniforms! Uniforms must be pushed by the user.
	RENDER_API void			render (
		CRenderableObject* owner, 
		RrMaterial* material,
		uchar pass,
		const IrrMeshBuilder& meshBuilder );
};

#endif//RENDERER_OBJECT_IMMEDIAT_RR_SCOPED_MESH_RENDERER_H_