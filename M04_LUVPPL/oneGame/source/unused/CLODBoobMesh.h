
#ifndef _C_BOOB_MESH__LOD__H_
#define _C_BOOB_MESH__LOD__H_

#include "CBoobMesh.h"

// == TERRAIN MESH ==
// The Boob mesh definition.
// This class controls the visual representation of its parent terrain boob, as well as its collision mesh.
class CLODBoobMesh : public CBoobMesh
{
public:
	typedef Terrain::terra_t terra_t;
	typedef Terrain::terra_t_lod terra_t_lod;

	// Data
	terra_t_lod*	m_dataSet;
	static terra_t_lod nullBlock;

	// Maximum values
	static const unsigned int maxVertexCount = (32768*2)-1;
public:
	// Constructor
	CLODBoobMesh ( void );
	// Destructor
	~CLODBoobMesh ( void );

	// Version 6 Regeneration
	//void AddBlockMesh ( CBoob *, char const, char const, short const, Vector3d const&, char const );
	//bool InSideVisible ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const&, char& );
	//terra_t_lod GetBlockValue ( CBoob *, char const, char const, terra_t_lod *, int const, int const, EFaceDir const, unsigned char ); 
	void AddBlockLOD ( const char indexx, const char indexy, const char indexz, const Vector3d& pos, const ftype size );
	terra_t_lod GetBlockValue ( const char indexx, const char indexy, const char indexz, const EFaceDir );

	// =Water mesh regeneration=
	/*bool PrepareWaterRegen ( void );
	void CleanupWaterRegen ( void );
	// Sends the built water mesh to memory
	void UpdateWaterVBOMesh ( void );
	// Version 1 Water Regeneration
	void GenerateInCubeWater ( CBoob *, char const, char const, int const, Vector3d const&, char const );
	//bool InSideVisibleWater ( CBoob *, char const, char const, char *, int const, int const, EFaceDir const, unsigned char, Vector3d const&, char& );
*/
};

#endif