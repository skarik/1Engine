

#ifndef _PHYS_STATIC_COMPRESSED_MESH_
#define _PHYS_STATIC_COMPRESSED_MESH_

// Include vertex data
#include "core/types/ModelData.h"

// Include the physics library
#include "physical/physics/CPhysics.h"
#include "physical/physics/shapes/physShape.h"

// Include string
#include <string>
using std::string;

// Class Definition
class physStaticCompressedMesh : public physShape
{
public:
	// Constructor
	PHYS_API physStaticCompressedMesh ( void );
	// Destructor
	PHYS_API ~physStaticCompressedMesh ( void );

	// 
	//	Initialize ( vertices, triangles, vertex count, triangle count, scale )
	// Update the collider with the given mesh.
	// If the vertex count is empty, will delete the current rigidbody. (This will wipe settings)
	// If no rigidbody exists and the vertex count is non-empty, will create a new rigidbody.
	PHYS_API void Initialize ( CTerrainVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count, Real scale );
	PHYS_API void Initialize ( CModelVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count, Real scale );

protected:
	void Initialize ( const hkGeometry* geometry );

private:
	bool	bReady;
};

#endif