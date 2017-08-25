
#include "engine/physics/motion/CRigidbody.h"
#include "physical/physics/shapes/physMesh.h"
#include "CConvexCollider.h"


//CConvexCollider::CConvexCollider ( string & sFilename )
CConvexCollider::CConvexCollider ( physMesh* collisionReference )
{
	//pCollisionShape = Physics::CreateBoxShape( string const& sFilename );
	/*const vector<physMesh*>* meshList = ModelMaster.GetCollisionReference( sFilename );

	if ( meshList != NULL )
	{
		if ( (*meshList)[0] != NULL )
		{
			pCollisionShape = (*meshList)[0]->GetShape();
		}
	}*/
	if ( collisionReference != NULL ) 
	{
		pCollisionShape = collisionReference->GetShape();;
	}
}
CConvexCollider::~CConvexCollider ( void )
{
	//Physics::FreeShape( pCollisionShape );
}
