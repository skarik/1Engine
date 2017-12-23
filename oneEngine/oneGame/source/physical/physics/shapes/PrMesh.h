#ifndef PHYSICAL_PHYSICS_MESH_
#define PHYSICAL_PHYSICS_MESH_

#include "core/types/ModelData.h"
#include "physical/physics/shapes/PrShape.h"

class PrMesh : public PrShape
{
public:
	PHYS_API explicit	PrMesh ( void );
	PHYS_API			~PrMesh ( void );

	//	Initialize(shapeData) : Actually creates the mesh data.
	// Will not clear previous data.
	PHYS_API void Initialize ( arModelPhysicsData* const pNewModelData, const bool force2Dunoptimized );

	// TODO:
	//PHYS_API void Free ( void );
};

#endif