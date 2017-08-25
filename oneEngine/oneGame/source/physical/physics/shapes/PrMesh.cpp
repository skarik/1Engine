#include "PrMesh.h"

// == Constructor ==
PrMesh::PrMesh ( void )
{
	bReady	= false;

	pmData	= NULL;
	meshNum	= 0;

	//pConvexShape	= NULL;
}

// == Destructor ==
PrMesh::~PrMesh ( void )
{
	if ( pmData != NULL )
	{
		if ( meshNum == 1 )
			delete [] pmData;
		else if ( meshNum > 1 )
			delete pmData;
		pmData = NULL;
		meshNum = 0;
	}
	/*if ( pConvexShape != NULL )
	{
		Physics::FreeShape( pConvexShape );
	}*/
}

// Init
void PrMesh::Initialize ( arModelPhysicsData* const pNewModelData )
{
	// Free the old data, duh
	//FreeVBOData ( );
	
	pmData = pNewModelData;
	//meshNum = frames;
	meshNum = 1;

	//pConvexShape = Physics::CreateConvMeshShape( pmData );
}