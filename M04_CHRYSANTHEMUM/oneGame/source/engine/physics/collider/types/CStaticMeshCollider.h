// Creates a mesh collider (meant to be a static mesh) from the given model object.
// The model object needs to have a model loaded for this to work.
// Note that this is not an object you want to be creating every step, as Havok's mesh compression causes a bit of latency with larger models.

#ifndef _C_STATIC_MESH_COLLIDER_H_
#define _C_STATIC_MESH_COLLIDER_H_

// Includes
//#include "CModel.h"
#include "core/types/ModelData.h"
#include "engine/physics/collider/CCollider.h"

// Class Definitions
class CStaticMeshCollider : public CCollider
{
public:
	// Constructor and destructor
	//explicit CStaticMeshCollider ( CModel* p_model, bool bUseRenderMesh = true, int iMeshIndex = 0 );
	ENGINE_API explicit CStaticMeshCollider ( CModelData* p_modeldata );
	ENGINE_API ~CStaticMeshCollider ( void );

protected:
	
};

#endif