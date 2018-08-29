#ifndef RENDERER_OBJECT_C_MESH_H_
#define RENDERER_OBJECT_C_MESH_H_

#include "core/math/BoundingBox.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"

class CModel;
class rrMeshBuffer;

//	class CMesh
// A CMesh provides a mechanism to draw a single rrMesh.
class CMesh : public CRenderableObject
{
public:
	RENDER_API explicit		CMesh ( rrMeshBuffer*, bool n_enableSkinning=false );
	RENDER_API virtual		~CMesh ( void );

public:
	//	GetName ()
	// Return my mesh's name
	RENDER_API const char* const
							GetName ( void ) const
		{ return m_name.c_str(); }

	//	GetBoundingBox ()
	// Gets the bounding box of the mesh
	RENDER_API BoundingBox	GetBoundingBox ( void ) const
		{ return bbCheckRenderBox; }

	//	GetCanRender ()
	// Get if frustum culling hides this mesh
	RENDER_API const bool	GetCanRender ( void ) const
		{ return bCanRender; }

	//		PreRender
	// Frustum culling & material Cbuffer update.
	RENDER_API bool			PreRender ( RrCamera* camera ) override;

	//		Render
	// Called during engine render pass.
	RENDER_API bool			Render ( const char pass ) override;

protected:
	// Get the mesh bounding box
	void					CalculateBoundingBox ( void );

public:
	rrMeshBuffer*		m_mesh;
	CModel*				m_parent;

protected:
	friend CModel;
	bool				bUseFrustumCulling;
	bool				bCanRender;
	bool				bUseSkinning;

	// Frustum Culling
	Vector3d			vCheckRenderPos;
	float				fCheckRenderDist;
	Vector3d			vMinExtents;
	Vector3d			vMaxExtents;
	BoundingBox			bbCheckRenderBox;

	arstring64			m_name;
};

#endif//RENDERER_OBJECT_C_MESH_H_