

#ifndef _C_CONVEX_COLLIDER_
#define _C_CONVEX_COLLIDER_

// Includes
#include "engine/physics/collider/CCollider.h"
//#include "CModelMaster.h"
class physMesh;

// Class Definition
class CConvexCollider : public CCollider
{
public:
	// Constructor and Destructor
	//CConvexCollider ( string & sFilename );
	CConvexCollider ( physMesh* collisionReference );
	~CConvexCollider ( void );

	// Setters
	//void SetSize ( Vector3d const& size );
	// Getters
	//Vector3d GetSize ( void );

protected:
	//Vector3d vExtents;

};


#endif