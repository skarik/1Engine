#ifndef PHYSICAL_PHYSICS_MESH_
#define PHYSICAL_PHYSICS_MESH_

#include "core/types/ModelData.h"
#include <string>

// Class Definition
class PrMesh
{
public:
	// Constructor
	PHYS_API PrMesh ( void );
	// Destructor
	PHYS_API ~PrMesh ( void );

	// 
	PHYS_API void Initialize ( arModelPhysicsData* const pNewModelData );

	// 
	//PHYS_API physShape* GetShape ( void );

private:
	bool	bReady;

	arModelPhysicsData* pmData;
	unsigned int meshNum;

	//physShape* pConvexShape;
};

#endif