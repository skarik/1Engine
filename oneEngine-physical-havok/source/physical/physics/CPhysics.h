//===============================================================================================//
// CPhysics.h
// Physics wrapper interface
//===============================================================================================//
#ifndef _C_PHYSICS_H_
#define _C_PHYSICS_H_

#include "CPhysicsCommon.h"

//===============================================================================================//
// Physics Class Wrapper Definition 
//===============================================================================================//
class CPhysics
{
public:
	enum eCastType_t
	{
		CASTRESULT_SINGLE,
		CASTRESULT_MULTIPLE
	};

public:
	PHYS_API CPhysics ( void ) {};
	PHYS_API CPhysics ( const CPhysics& ) {};
private:
	PHYS_API static CPhysics* Instance;
	//PHYS_API void TestFunc ( float time );
public:
	PHYS_API static void Init ( void );
	PHYS_API static void Free ( void );
	//static void Update ( float, CGameState* pGameState, void (CGameState::*pt2FixedUpdate)(void), void(CGameState::*pt2RigidbodyUpdate)(void) );
	//static void UpdateThreaded ( float frameDeltaTime, float fixedDeltaTime, CGameState* pGameState, void (CGameState::*pt2FixedUpdate)(void), void(CGameState::*pt2RigidbodyUpdate)(void) );
	PHYS_API static CPhysics* Active ( void );
	PHYS_API static void SetActive ( CPhysics* );
private:
	void _Init ( void );
	void _Free ( void );

private:
	void InitMultithreading ( void );
	void InitLayers ( void );
	
public:
	//=========================================//
	// Modifying the Physics World
	//=========================================//
	FORCE_INLINE PHYS_API static void ShiftWorld ( Vector3d vShift );
	FORCE_INLINE PHYS_API static void GetWorldCenter ( Vector3d& vWorldPosition );

	FORCE_INLINE PHYS_API static Vector3d GetWorldGravity ( void );

	//=========================================//
	// World Update
	//=========================================//
	FORCE_INLINE PHYS_API static void UpdateSimulationTarget ( float deltaTime );
	FORCE_INLINE PHYS_API static bool SimulationAtTarget ( void );
	FORCE_INLINE PHYS_API static bool Simulate ( float simulationTime );
	FORCE_INLINE PHYS_API static bool SimulationAtSubstep ( void );

	//=========================================//
	// Collision Shapes
	//=========================================//
	// Creation of shapes for collision
	FORCE_INLINE PHYS_API static physShape* CreateBoxShape ( Vector3d vHalfExtents );
	FORCE_INLINE PHYS_API static physShape* CreateBoxShape ( Vector3d vHalfExtents, Vector3d vCenterOffset );
	FORCE_INLINE PHYS_API static physShape* CreateCapsuleShape ( Vector3d vStart, Vector3d vEnd, float fRadius );
	FORCE_INLINE PHYS_API static physShape* CreateCylinderShape ( Vector3d vStart, Vector3d vEnd, float fRadius, float fConvexRadius=0.01f );
	FORCE_INLINE PHYS_API static physShape* CreateSphereShape ( float fRadius );
	FORCE_INLINE PHYS_API static physShape* CreateTriangleShape ( Vector3d* vpPositions, float fExtrusion=0.01f );
	FORCE_INLINE PHYS_API static physShape* CreateConvMeshShape ( CPhysicsData* pMesh );
	FORCE_INLINE PHYS_API static physShape* CreateMeshShape ( CPhysicsData* pMesh );
	FORCE_INLINE PHYS_API static physShape* CreateMeshShape ( CTerrainVertex* pVertices, CModelQuad* pQuads, unsigned short vertexCount, unsigned short faceCount );
	FORCE_INLINE PHYS_API static physShape* CreateMeshShape ( CModelData const* pMesh );
	FORCE_INLINE PHYS_API static physShape* CreateFastMeshShape ( Vector3d* pVertices, CModelTriangle* pTris, unsigned short vertexCount, unsigned short faceCount );
	// Creation of static meshes
	//inline static int CreateMeshConcave ( CPhysicsData const&, CTransform const& );
	FORCE_INLINE PHYS_API static physShape* CreateEMesh ( CPhysicsVertex* pMesh, CTransform const& );
	// Freeing shapes
	FORCE_INLINE PHYS_API static void FreeShape ( physShape* pShape );

	//=========================================//
	// Rigidbodies
	//=========================================//
	// Creation of rigidbodies
	FORCE_INLINE PHYS_API static hkpRigidBody* CreateRigidBody ( physRigidBodyInfo* pBodyInfo, bool bIsDynamic=true );
	// Destruction of rigidbodies
	FORCE_INLINE PHYS_API static void FreeRigidBody ( hkpRigidBody* pRigidBody );
	// Checking for rigidbody collisions
	FORCE_INLINE PHYS_API static void CheckRigidBodyContacts ( hkpRigidBody* pRigidBody );
	// Setting their info
	FORCE_INLINE PHYS_API static void SetRigidBodyTransform ( hkpRigidBody* pRigidBody, CTransform* pSourceTransform );
	// Grabbing their info
	FORCE_INLINE PHYS_API static void GetRigidBodyTransform ( hkpRigidBody* pRigidBody, CTransform* pTargetTransform );
	FORCE_INLINE PHYS_API static void GetRigidBodyTranslation( hkpRigidBody* pRigidBody, CTransform* pTargetTransform );

	//=========================================//
	// Phantoms
	//=========================================//
	// Creation of phantoms
	FORCE_INLINE PHYS_API static physCollisionVolume* CreateAABBPhantom ( hkAabb* pInfo, unsigned int iOwnerID );
	FORCE_INLINE PHYS_API static physCollisionVolume* CreateShapePhantom ( const physShape* pShape, CTransform* pSourceTransform, unsigned int iOwnerID );
	// Destruction of phantoms
	FORCE_INLINE PHYS_API static void FreePhantom ( physCollisionVolume* pCollisionVolume );
	// Checking for phantom collisions contacts
	FORCE_INLINE PHYS_API static void CheckPhantomContacts ( physCollisionVolume* pCollisionVolume );
	//==Trigger Phantoms==
	// Creation of a trigger phantom
	FORCE_INLINE PHYS_API static hkpRigidBody* CreateTriggerVolume ( physRigidBodyInfo* pBodyInfo, const physShape* pShape, hkpPhantomCallbackShape* pCbPhantom );
	// Modifying phantoms
	FORCE_INLINE PHYS_API static void SetPhantomAABB ( hkpAabbPhantom* phantom, hkAabb* aabb );

	//=========================================//
	// Tracing Collision Queries
	//=========================================//
	// Cast a ray
	//PHYS_API static void Raycast ( Ray const& rDir, ftype fCastDist, RaycastHit * outHitInfo, uint32_t collisionFilter = 0, void* mismatch=NULL );
	//FORCE_INLINE PHYS_API static void Raycast( const hkpWorldRayCastInput& input, hkpRayHitCollector& collector );
	FORCE_INLINE PHYS_API static void Raycast ( const physCollisionFilter& filter, const Ray& ray, const eCastType_t type, std::vector<std::pair<ContactPoint,uint32_t>>& pointCollection );
	FORCE_INLINE PHYS_API static void INTERNAL_Raycast ( const hkpWorldRayCastInput& input, hkpRayHitCollector& collector );
	// Cast a shape
	//PHYS_API static void Linearcast ( Ray const& rDir, ftype fCastDist, physShape* pShape, RaycastHit* outHitInfo, const int hitInfoArrayCount, uint32_t collisionFilter = 0, void* mismatch=NULL );
	//FORCE_INLINE PHYS_API static void Linearcast( const hkpCollidable* collA, const hkpLinearCastInput& input, hkpCdPointCollector& castCollector, hkpCdPointCollector* startCollector = HK_NULL );
	FORCE_INLINE PHYS_API static void Linearcast( const physCollisionFilter& filter, const physShape* shape, const Ray& ray, std::vector<std::pair<ContactPoint,uint32_t>>& pointCollection );
	//FORCE_INLINE PHYS_API static void Linearcast( const physCollisionFilter& filter, const physShape* shape, const Ray& ray, hkpCdPointCollector& castCollector, hkpCdPointCollector* startCollector = HK_NULL );

	// Get closest points to a collider
	//FORCE_INLINE PHYS_API static void GetClosestPoints ( const hkpCollidable* collA, const hkpCollisionInput& input, hkpCdPointCollector& collector );
	FORCE_INLINE PHYS_API static void GetClosestPoints ( const physCollisionFilter& filter, const physShape* shape, const Vector3d& input, std::vector<std::pair<ContactPoint,uint32_t>>& pointCollection );


	//=========================================//
	// Collision
	//=========================================//
	// Get collision filter
	FORCE_INLINE PHYS_API static uint32_t GetCollisionFilter ( int layer, int subsystem = -1, int nocollidewith = -1 );

	// Get a collision collector
	FORCE_INLINE PHYS_API static hkpCollisionInput* GetCollisionCollector ( void );

	//=========================================//
	// Object handling
	//=========================================//
	FORCE_INLINE PHYS_API static void AddEntity ( hkpEntity* entity );
	FORCE_INLINE PHYS_API static void ForceEntityUpdate ( hkpEntity* entity );

	FORCE_INLINE PHYS_API static void AddConstraint ( hkpConstraintInstance* constraint );

	FORCE_INLINE PHYS_API static void AddPhantom ( hkpPhantom* phantom );
	FORCE_INLINE PHYS_API static void AddListener ( hkpWorldPostSimulationListener* listener );

	FORCE_INLINE PHYS_API static void RemoveReference ( hkReferencedObject* object );

	//=========================================//
	// Threading
	//=========================================//
	FORCE_INLINE PHYS_API static void ThreadLock ( void );
	FORCE_INLINE PHYS_API static void ThreadUnlock ( void );
	FORCE_INLINE PHYS_API static void ReadLock ( void );
	FORCE_INLINE PHYS_API static void ReadUnlock ( void );

private:
	//=========================================//
	// System Query
	//=========================================//
	PHYS_API static hkpWorld* World ( void );

	PHYS_API static hkJobQueue* JobQueue ( void );
	PHYS_API static hkJobThreadPool* ThreadPool ( void );

#ifdef _HAVOK_VISUAL_DEBUGGER_
	PHYS_API static hkVisualDebugger* VDB ( void );
#endif

private:
	// System
	hkMallocAllocator	baseMalloc;
	hkMemoryRouter*		memoryRouter;
	// Physics
	hkpWorldCinfo		worldInfo;
	hkpWorld*			pWorld;

	// Multithreading
	hkJobThreadPool*	threadPool;
	hkJobQueue*			jobQueue;
	int					totalNumThreadsUsed;

	// World Properties
	Vector3d			vWorldCenter;

#ifdef _HAVOK_VISUAL_DEBUGGER_
	// Debugger
	hkpPhysicsContext*	physicsContext;
	hkVisualDebugger*	vdb;
#endif

	// Collision filtering
	int*		systemGroups;
};

typedef CPhysics Physics;
//#define PhysicsEngine. CPhysics::

#include "CPhysics.hpp"

#endif