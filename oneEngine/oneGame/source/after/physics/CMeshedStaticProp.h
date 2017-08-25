//===============================================================================================//
//	class CMeshedStaticProp
//
// Helper class for creating a static rigidbody from pure mesh data.
// Used for static procedural objects that need	
//===============================================================================================//

//=========================================//
// Includes 

#include "core/types/ModelData.h"
#include "engine/behavior/CGameBehavior.h"

#include "physical/physics.h"
#include "physical/physics/shapes/physShape.h"

//=========================================//
// Classes

class CMeshedStaticProp
{
public:
	//=========================================//
	// Constructor + Destructor

	explicit CMeshedStaticProp ( void );
			~CMeshedStaticProp ( void );

	//=========================================//
	// Public Updaters / Setters

	//	UpdateScale ( new_scale )
	// Change the scale that the mesh is loaded up with
	void UpdateScale ( const float n_newScale );

	//	UpdateCollider ( vertices, triangles, vertex count, triangle count )
	// Update the collider with the given mesh.
	// If the vertex count is empty, will delete the current rigidbody. (This will wipe settings)
	// If no rigidbody exists and the vertex count is non-empty, will create a new rigidbody.
	void UpdateCollider ( CTerrainVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count );
	void UpdateCollider ( CModelVertex* vertices, CModelTriangle* triangles, uint vertex_count, uint triangle_count );

	//	SetOwner ( new_owner )
	// Update's the rigidbody userdata with the ID of the given behavior.
	void SetOwner ( CGameBehavior* n_newBehavior );

	//	SetPosition ( new_position )
	// Change the position of the rigidbody
	void SetPosition ( const Vector3d& n_newPosition );

protected:
	//=========================================//
	// Private Properties

	physRigidBody*	m_rigidbody;
	float			m_scale;

	//=========================================//
	// Privated Helpers

	//	UpdateColliderCommon ( geometry )
	// Actually performs the rigidbody creation/updating using the given geometry.
	void UpdateColliderCommon ( hkGeometry& n_geometryToUse );

	//	DeleteRigidbody ()
	// Not much to say. Deletes the rigidbody.
	void DeleteRigidbody ( void );
};