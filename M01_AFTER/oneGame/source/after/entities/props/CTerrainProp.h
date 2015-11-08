
#ifndef _C_TERRA_COMPONENT_H_
#define _C_TERRA_COMPONENT_H_

// Include Actor
#include "engine-common/entities/CActor.h"
// Include Model
/*#include "CModel.h"
// Include Rigidbody
#include "CRigidbody.h"
// Include Box Collider
#include "CBoxCollider.h"
#include "CStaticMeshCollider.h"
// Declare terrain prototypes
class CBoob;
struct subblock16;
struct subblock8;
// Include BlockInfo struct
#include "BlockInfo.h"
// Include RaycastHit struct
#include "RaycastHit.h"*/

#include "renderer/logic/model/CModel.h"
#include "renderer/object/screen/CGlowObject.h"

#include "engine/physics/motion/CRigidbody.h"
#include "engine/physics/collider/types/CBoxCollider.h"
#include "engine/physics/collider/types/CStaticMeshCollider.h"

#include "after/types/terrain/BlockTracker.h"


// Define Class
class CTerrainProp abstract : public CActor
{
	ClassName( "CTerrainProp" );
	BaseClass( "CGameObject_TerrainProp" );
public:
	explicit CTerrainProp ( BlockTrackInfo const& inInfo )
		: CActor()
	{
		mBlockInfo = inInfo;

		AddToTerrain();

		m_body = NULL;
		m_collider = NULL;
		m_model = NULL;
		m_modelGlow = NULL;

		mCollidesWithPlayer = false;
		mUseConvexCollision = true;
	}
	// Destructor for freeing up owned pointers
	virtual ~CTerrainProp ( void )
	{
		RemoveFromTerrain();

		if ( m_body )
			delete m_body;
		m_body = NULL;
		if ( m_collider )
			delete m_collider;
		m_collider = NULL;
		if ( m_modelGlow )
			delete m_modelGlow;
		m_modelGlow = NULL;
		if ( m_model )
			delete m_model;
		m_model = NULL;
	}

	// Function for initializing the collision/physics
	// If this function is not overloaded, it will simply create a bounding box based on the bounding box of...well, it creates a bounding box.
	// Override this function is you want to have your own collision mesh on the weapon
	// Is based on CWeaponItem's physics
	virtual void CreatePhysics ( void )
	{
		if ( m_model == NULL )
		{
			std::cout << "Problem in CTerrainProp (" << GetTypeName() << ") mem(" << (void*)(this) << ") " << std::endl;
			std::cout << "Cannot create a physics presence without a model attached!" << std::endl;
		}
		else
		{
			// Create a box collider based on the model's bounding box,
			//  but only create it if there's no collider already
			if ( m_collider == NULL ) {
				if ( mUseConvexCollision ) {
					m_collider = new CBoxCollider( m_model->GetBoundingBox(), Vector3d(0,0,0) );
				}
				else {
					//m_collider = new CMeshColl
					//m_collider = new CStaticMeshCollider( m_model, true, 0 );
					m_collider = new CStaticMeshCollider( m_model->GetModelData(0) );
				}
			}

			// Create a rigidbody based on the collider
			m_body = new CRigidBody( m_collider, this );

			// Update the body's position
			/*m_body->SetPosition( transform.position );
			m_body->SetRotation( transform.rotation );*/
			transform.SetDirty();

			// Change the body's friction
			m_body->SetFriction( 1.6f );

			// Change the body's motion type
			//m_body->SetQualityType( HK_COLLIDABLE_QUALITY_CRITICAL );

			// Change the body to kinematic
			m_body->SetMotionType( physMotion::MOTION_KEYFRAMED );

			// Change collision type to item
			if ( mCollidesWithPlayer )
				m_body->SetCollisionLayer( Layers::PHYS_DYNAMIC );
			else
				m_body->SetCollisionLayer( Layers::PHYS_ITEM );
		}
	}

	// On creation and placement in the terrain. Cool stuff.
	virtual void OnTerrainPlacement ( void ) {;};

	// Pre-simulation. Is executed after the electric current map clears. Targeted at 10 frames per second (or half the framerate, whatever is lower).
	// Is not "thread-safe"
	virtual void PreSimulation ( void ) {};
	// Post-simulation. Is executed after all simulation has finished. Targeted at 10 frames per second (or half the framerate, whatever is lower).
	// Is not "thread-safe"
	virtual void PostSimulation ( void ) {};

	// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
	virtual void OnPunched ( RaycastHit const& hitInfo ) {};

	// Get userdata. Overridable for save/load specific data.
	virtual uint64_t GetUserdata ( void )
	{
		return 0;
	}
	// Set userdata. Overrideable for save/load specific data.
	virtual void SetUserdata ( const uint64_t& )
	{
		;
	}

	// Lateupdate
	void LateUpdate ( void )
	{
		if ( m_body == NULL )
		{
			CreatePhysics();
		}
		// Update model position
		if ( m_model ) {
			m_model->transform.Get( transform );
			// Update model glow
			if ( m_modelGlow == NULL ) {
				m_modelGlow = new CGlowObject ( m_model );
				m_modelGlow->SetVisible( false );
			}
		}
	}

	// Get current block info
	BlockTrackInfo	GetBlockInfo ( void )
	{
		return mBlockInfo;
	}

protected:
	bool		mCollidesWithPlayer;
	bool		mUseConvexCollision;

protected:
	// Data
	BlockTrackInfo	mBlockInfo;

	// Pointers to physical and visual presense of the item
	CRigidBody*	m_body;
	CCollider*	m_collider;
	CModel*		m_model;
	CGlowObject*m_modelGlow;

	// Private default constructor to not call base classes
	CTerrainProp ( char const ) { ;};

	// Helper functions to add/remove components from terrain
	// Uses data stored in mBlockInfo
	void RemoveFromTerrain ( void );
	void AddToTerrain ( void );
};

#endif