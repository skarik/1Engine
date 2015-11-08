
#ifndef _C_BOOB_METABOOB_H_

#include "CBoob.h"

// == TERRAIN METABOOBS ==
// This is a solely storage version of the boob.
// While it has all the same pointers, it doesn't create any meshes.
class CMetaboob : public CBoob
{
public:
	// Additional storage for straight-to-hdd generation

	// Foliage IO, mirroring what's stored in the terrain files.
	struct sFoliageIO
	{
		unsigned short foliage_index;
		Vector3d	position;
		//uint64_t	bitmask;
		//string		userdata;
		char		userdata[48];
	};
	// List of IO foliage
	vector<sFoliageIO> v_foliageIO;

	// Component IO, mirrors what's stored in the terrain files.
	struct sComponentIO
	{
		unsigned short component_index;
		Vector3d	position;
		Vector3d	rotation;
		Vector3d	scaling;
		uint64_t	data;
		char		b16index;
		char		b8index;
		short		bindex;
	};
	// List of IO components
	vector<sComponentIO> v_componentIO;

public:
	CMetaboob ( )
	{
		// Set pointers to null
		pMesh = NULL;
		pGrass = NULL;
		//cout << "New CMetaboob" << endl;
	}
	~CMetaboob ( )
	{
		// Nothing
	}
};

#endif