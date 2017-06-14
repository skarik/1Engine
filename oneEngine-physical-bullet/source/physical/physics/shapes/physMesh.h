

#ifndef _PHYS_MESH_
#define _PHYS_MESH_

// Include vertex data
#include "core/types/ModelData.h"

// Include the physics library
#include "physical/physics/CPhysics.h"

// Include string
#include <string>
using std::string;

// Class Definition
class physMesh
{
public:
	// Constructor
	PHYS_API physMesh ( void );
	// Destructor
	PHYS_API ~physMesh ( void );

	// 
	PHYS_API void Initialize ( arModelPhysicsData* const pNewModelData );

	// 
	PHYS_API physShape* GetShape ( void );

private:
	bool	bReady;

	arModelPhysicsData* pmData;
	unsigned int meshNum;

	physShape* pConvexShape;
};

#endif