//===============================================================================================//
// CPhysics.h
// Physics wrapper interface
//===============================================================================================//
#ifndef _C_PHYSICS_H_
#define _C_PHYSICS_H_

#include "CPhysicsCommon.h"
#include <mutex>

//===============================================================================================//
// Physics Class Wrapper Definition 
//===============================================================================================//
class CPhysics
{
public:
	PHYS_API CPhysics ( void );
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
	// Debugger
	//=========================================//
	PHYS_API static void SetDebugRenderer ( b2Draw* physicsDebugRenderer );
	PHYS_API static void RenderDebugInfo ( void );

	//=========================================//
	// Modifying the Physics World
	//=========================================//
	FORCE_INLINE PHYS_API static Vector3d WorldScaling ( void );

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
	FORCE_INLINE PHYS_API static physShape* CreateConvMeshShape ( arModelPhysicsData* pMesh );
	FORCE_INLINE PHYS_API static physShape* CreateMeshShape ( arModelPhysicsData* pMesh );
	FORCE_INLINE PHYS_API static physShape* CreateMeshShape ( arTerrainVertex* pVertices, arModelQuad* pQuads, unsigned short vertexCount, unsigned short faceCount );
	FORCE_INLINE PHYS_API static physShape* CreateMeshShape ( arModelData const* pMesh );
	FORCE_INLINE PHYS_API static physShape* CreateFastMeshShape ( Vector3d* pVertices, arModelTriangle* pTris, unsigned short vertexCount, unsigned short faceCount );
	// Creation of static meshes
	//inline static int CreateMeshConcave ( arModelPhysicsData const&, CTransform const& );
	FORCE_INLINE PHYS_API static physShape* CreateEMesh ( arPhysicsVertex* pMesh, core::Transform const* );
	// Freeing shapes
	FORCE_INLINE PHYS_API static void FreeShape ( physShape* pShape );

	//=========================================//
	// Rigidbodies
	//=========================================//
	// Creation of rigidbodies
	FORCE_INLINE PHYS_API static b2Body* CreateRigidBody ( physRigidBodyInfo* pBodyInfo, bool bIsDynamic=true );
	// Destruction of rigidbodies
	FORCE_INLINE PHYS_API static void FreeRigidBody ( b2Body* pRigidBody );
	// Checking for rigidbody collisions
	FORCE_INLINE PHYS_API static void CheckRigidBodyContacts ( physRigidBody* pRigidBody );
	// Setting their info
	//FORCE_INLINE PHYS_API static void SetRigidBodyTransform ( physRigidBody* pRigidBody, CTransform* pSourceTransform );
	// Grabbing their info
	//FORCE_INLINE PHYS_API static void GetRigidBodyTransform ( physRigidBody* pRigidBody, CTransform* pTargetTransform );
	//FORCE_INLINE PHYS_API static void GetRigidBodyTranslation( physRigidBody* pRigidBody, CTransform* pTargetTransform );

	//=========================================//
	// Phantoms
	//=========================================//
	// Creation of phantoms
	FORCE_INLINE PHYS_API static physCollisionVolume* CreateAABBPhantom ( physAabb* pInfo, unsigned int iOwnerID );
	FORCE_INLINE PHYS_API static physCollisionVolume* CreateShapePhantom ( physShape* pShape, core::Transform* pSourceTransform, unsigned int iOwnerID );
	// Destruction of phantoms
	FORCE_INLINE PHYS_API static void FreePhantom ( physCollisionVolume* pCollisionVolume );
	// Checking for phantom collisions contacts
	FORCE_INLINE PHYS_API static void CheckPhantomContacts ( physCollisionVolume* pCollisionVolume );
	// Modifiy shapes
	FORCE_INLINE PHYS_API static void SetPhantomAABB( physCollisionVolume* m_phantom, physAabb* m_aabb );
	//==Trigger Phantoms==
	// Creation of a trigger phantom
	FORCE_INLINE PHYS_API static physRigidBody* CreateTriggerVolume ( physRigidBodyInfo* pBodyInfo, physShape* pShape, physPhantomCallbackShape* pCbPhantom );

	//=========================================//
	// Tracing Collision Queries
	//=========================================//
	// Cast a ray
	//PHYS_API static void Raycast ( Ray const& rDir, Real fCastDist, RaycastHit * outHitInfo, uint32_t collisionFilter = 0, void* mismatch=NULL );
	FORCE_INLINE PHYS_API static void Raycast( const physWorldRayCastInput& input, physRayHitCollector& collector );
	// Cast a shape
	//PHYS_API static void Linearcast ( Ray const& rDir, Real fCastDist, physShape* pShape, RaycastHit* outHitInfo, const int hitInfoArrayCount, uint32_t collisionFilter = 0, void* mismatch=NULL );
	FORCE_INLINE PHYS_API static void Linearcast( const physCollidable* collA, const physLinearCastInput& input, physCdPointCollector& castCollector, physCdPointCollector* startCollector = HK_NULL );

	//=========================================//
	// Collision
	//=========================================//
	// Get collision filter
	//FORCE_INLINE PHYS_API static uint32_t GetCollisionFilter ( int layer, int subsystem = -1, int nocollidewith = -1 );
	FORCE_INLINE PHYS_API static physCollisionFilter GetCollisionFilter ( int layer, int subsystem = -1, int nocollidewith = -1 );

	// Get a collision collector
	FORCE_INLINE PHYS_API static physCollisionInput* GetCollisionCollector ( void );

	// Get closest points to a collider
	FORCE_INLINE PHYS_API static void GetClosestPoints ( const physCollidable* collA, const physCollisionInput& input, physCdPointCollector& collector );

	//=========================================//
	// Object handling
	//=========================================//
	FORCE_INLINE PHYS_API static void AddEntity ( physEntity* entity );
	FORCE_INLINE PHYS_API static void ForceEntityUpdate ( physEntity* entity );

	FORCE_INLINE PHYS_API static void AddConstraint ( physConstraintInstance* constraint );

	FORCE_INLINE PHYS_API static void AddPhantom ( physPhantom* phantom );
	FORCE_INLINE PHYS_API static void AddListener ( physWorldPostSimulationListener* listener );

	FORCE_INLINE PHYS_API static void RemoveReference ( physObject* entity );

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
	PHYS_API static physWorld* World ( void );
/*
	PHYS_API static hkJobQueue* JobQueue ( void );
	PHYS_API static hkJobThreadPool* ThreadPool ( void );

#ifdef _HAVOK_VISUAL_DEBUGGER_
	PHYS_API static hkVisualDebugger* VDB ( void );
#endif
	*/
private:
	// System
	/*hkMallocAllocator	baseMalloc;
	hkMemoryRouter*		memoryRouter;
	// Physics
	hkpWorldCinfo		worldInfo;
	hkpWorld*			pWorld;

	// Multithreading
	hkJobThreadPool*	threadPool;
	hkJobQueue*			jobQueue;
	int					totalNumThreadsUsed;
	*/
	b2World*			pWorld;
	b2Draw*				pWorldDebugger;

	// World Properties
	Vector3d			vWorldCenter;


	Vector3d			worldScaling;

	Real				targetTime;
	Real				worldTime;

	std::mutex			mutexRead;
	std::mutex			mutexWrite;
	/*
#ifdef _HAVOK_VISUAL_DEBUGGER_
	// Debugger
	hkpPhysicsContext*	physicsContext;
	hkVisualDebugger*	vdb;
#endif
	*/
	// Collision filtering
	//int*		systemGroups;
	uint16_t*	collisionMasks;
};

typedef CPhysics Physics;
//#define PhysicsEngine. CPhysics::

#include "CPhysics.hpp"

#endif