#ifndef RENDERER_OBJECT_C_MESH_H_
#define RENDERER_OBJECT_C_MESH_H_

#include "core/math/BoundingBox.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/object/mesh/system/rrMesh.h"

class CModel;

class CMesh : public CRenderableObject
{
public:
	RENDER_API explicit	CMesh ( rrMesh*, bool n_enableSkinning=false );
	RENDER_API virtual ~CMesh ( void );

public:
	//	GetName ()
	// Return my mesh's name
	const string& GetName ( void )
		{ return m_glMesh->GetName(); }

	//	GetBoundingBox ()
	// Gets the bounding box of the mesh
	BoundingBox	GetBoundingBox ( void ) const
		{ return bbCheckRenderBox; }

	//	GetCanRender ()
	// Get if frustum culling hides this mesh
	const bool	GetCanRender ( void ) const
		{ return bCanRender; }

	//		PreRender
	// Frustum culling & material Cbuffer update.
	RENDER_API bool	PreRender ( void ) override;

	//		Render
	// Called during engine render pass.
	RENDER_API bool	Render ( const char pass ) override;

protected:
	// Get the mesh bounding box
	void CalculateBoundingBox ( void );

public:
	rrMesh*		m_glMesh;
	CModel*		m_parent;

protected:
	friend CModel;
	bool		bUseFrustumCulling;
	bool		bCanRender;
	bool		bUseSkinning;

	// Frustum Culling
	Vector3d	vCheckRenderPos;
	float		fCheckRenderDist;
	Vector3d	vMinExtents;
	Vector3d	vMaxExtents;
	BoundingBox bbCheckRenderBox;

};

#endif//RENDERER_OBJECT_C_MESH_H_