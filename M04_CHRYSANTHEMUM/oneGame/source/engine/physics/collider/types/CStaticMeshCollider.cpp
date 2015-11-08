
// Includes
#include "CStaticMeshCollider.h"

// Constructor + Destructor
/*CStaticMeshCollider::CStaticMeshCollider ( CModel* p_model, bool bUseRenderMesh, int iMeshIndex )
{
	pCollisionShape = Physics::CreateMeshShape( p_model->GetModelData( iMeshIndex ) );
}*/

CStaticMeshCollider::CStaticMeshCollider ( CModelData* p_modeldata )
{
	pCollisionShape = Physics::CreateMeshShape( p_modeldata );
}

CStaticMeshCollider::~CStaticMeshCollider ( void )
{
	Physics::FreeShape( pCollisionShape );
}
