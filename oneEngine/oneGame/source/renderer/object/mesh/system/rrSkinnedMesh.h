// Mesh Interface.
// Pass it some vertices and move its bones. It will render them.
// THAT'S ALL IT DOES. TRANSFORMATIONS AND MATERIALS MUST BE APPLIED ELSEWHERE BEFORE RENDERING
#ifndef RR_SKINNED_MESH_H_
#define RR_SKINNED_MESH_H_

#include "rrMesh.h"
#include "physical/skeleton/skeletonBone.h"

// Class Definition
class rrSkinnedMesh : public rrMesh
{
public:
	// Constructor
	rrSkinnedMesh ( void );
	// Destructor
	virtual ~rrSkinnedMesh ( void );

	//	Initialize ( name, data ) : sets up new mesh data
	// Creates a new VBO associated with this rrMesh
	// Removes any old VBO data.
	void Initialize ( const char* nNewName, arModelData* const pNewModelData, unsigned int frames=1, bool isStreamed=false ) override;

	//	Copy ( ) : creates a new rrSkinnedMesh with duplicated streams
	rrSkinnedMesh* Copy ( void );

	// Render mode
	enum eRenderMode_t
	{
		RD_GPU = 0,
		RD_CPU,
		RD_GPU_FALLBACK //for modes that do not support GPU pallettes (splits up mesh, ala doom3)
	};
	eRenderMode_t		iRenderMode;

	// Quality Type
	enum eQualityType_t
	{
		QD_HIGH = 0,
		QD_MED,
		QD_LOW
	};
	eQualityType_t		iQualityType;

	// Set skeleton pointer
	void SetSkinningData ( const skinningData_t& n_skinning_data );

	void UpdateVBO ( void );

protected:
	bool		useSkinning;

public:
	skinningData_t	skinning_data;
};

#endif//RR_SKINNED_MESH_H_