
#ifndef _C_TERRA_GRASS_
#define _C_TERRA_GRASS_

// Includes
#include "core/math/Vector2d.h"
#include "core/types/ModelData.h"
#include "renderer/types/types.h"
#include "renderer/object/CRenderableObject.h"
#include "after/types/terrain/grass.h"

// Temp class def
class CCamera;

// Classes
class CTerraGrass : public CRenderableObject
{

public:
	explicit CTerraGrass ( std::vector<Terrain::grass_t>* );
	~CTerraGrass ( void );

	// Generates the mesh, sorted from back-to-front order
	bool PreRender ( const char pass );
	// Sends the VBO and renders
	bool Render ( const char pass );

	// Simulates the grass patch
	void Simulate ( void );

	// Breaks a grass on the target block
	void BreakGrass ( Terrain::terra_b* target );
	// Check if target block is in grass list
	bool HasGrass ( Terrain::terra_b* target );

	// Owner boob
	//CBoob*	pOwnerBoob;
	// List of grass
	std::vector<Terrain::grass_t>*	pv_grasslist;

private:
	// == Private Members ==
	// OpenGL handles
	glHandle	grassVerts;
	glHandle	grassFaces;

	// Current mesh
	CTerrainVertex*	pVertices;
	CModelQuad*		pQuads;
	uint			iVertices;
	uint			iQuads;
	uint			iMaxQuads;
	uint			iGrassRes;

	// Mesh generation values
	ftype			genfHeight;
	ftype			genfUVOffset;
	ftype			genfLight;

	// Wind direction
	Vector2d	vWindDir;

	// == Private Sorter ==
	// Comparison structure
	struct sTerraGrassSorter
	{
		Vector3d offset;
		bool operator() ( Terrain::grass_t &, Terrain::grass_t & ); // returns true when i < j
	};

	// == Private Functions ==
	// Frees VBO data
	void FreeVBOData ( void );
	
	// Adds a grass patch at the given position
	void AddGrassPatch ( const Vector3d &, ushort type );
	void AddHighresGrassPatch ( const Vector3d &, ushort type );

	// == Static Members ==
	// Common sorter
	static sTerraGrassSorter mySorter;
	// Static Materials
	static glMaterial* grassMaterial;
	// Init static
	static void InitMaterials ( void );
};

#endif