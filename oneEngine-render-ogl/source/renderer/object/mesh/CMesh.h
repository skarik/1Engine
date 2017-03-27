
#ifndef _C_MESH_H_
#define _C_MESH_H_

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
	// Return my mesh's name
	const string& GetName ( void ) {
		return m_glMesh->GetName();
	}

	// Gets the bounding box of the mesh
	BoundingBox GetBoundingBox ( void ) const {
		return bbCheckRenderBox;
	}

	// Get if frustum culling hides this mesh
	const bool GetCanRender ( void ) const {
		return bCanRender;
	}
protected:
	// Get the mesh bounding box
	void CalculateBoundingBox ( void );

public:
	rrMesh*		m_glMesh;
	CModel*		m_parent;

	// Frustom Culling Check
	bool PreRender ( const char pass );
	// Public Render Routine
	bool Render ( const char pass );

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

#endif//_C_MESH_H_