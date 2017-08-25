#ifndef _C_PHYSICS_COMMON_
#define _C_PHYSICS_COMMON_

#include "physical/mode.h"

// Game System Includes
#include "core/types/types.h"
#include "core/types/ModelData.h"
#include "core-ext/transform/Transform.h"
#include "core/debug/CDebugConsole.h"

// Raytracing includes
#include "core/math/Ray.h"
#include "physical/physics/cast/RaycastHit.h"
#include "physical/physics/shapes/physShape.h"

// Havok Common Includes
/*#include <Common/Base/hkBase.h>
#include <Common/Base/Container/Array/hkArray.h> 
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Fwd/hkcstdio.h>
#include <Common/GeometryUtilities/Misc/hkGeometryUtils.h>
// Havok Thread Includes
#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

// Havok Physics Includes
#include <Common/Base/Types/Geometry/hkStridedVertices.h> 

#include <Physics/Dynamics/hkpDynamics.h>
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/Phantom/hkpPhantom.h> 
#include <Physics/Dynamics/Phantom/hkpSimpleShapePhantom.h>
#include <Physics/Collide/Agent/hkpCollisionInput.h>			// Collision input class
#include <Physics/Collide/BroadPhase/hkpBroadPhase.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h> 
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Misc/Bv/hkpBvShape.h>			// Compound shape object
#include <Physics/Collide/Shape/Misc/PhantomCallback/hkpPhantomCallbackShape.h>		// Volume/Shape based trigger
#include <Physics/Collide/Shape/Query/hkpShapeRayCastInput.h>						// Raycast
#include <Physics/Collide/Shape/Query/hkpShapeRayCastOutput.h>						// Raycast
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>					// Raycast
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>					// Raycast
#include <Physics/Collide/Query/Collector/RayCollector/hkpClosestRayHitCollector.h>	// Raycast
#include <Physics/Collide/Query/Collector/RayCollector/hkpAllRayHitCollector.h>		// Raycast
#include <Physics/Collide/Query/CastUtil/hkpLinearCastInput.h>						// Linecast
#include <Physics/Collide/Query/Collector/PointCollector/hkpClosestCdPointCollector.h>		// Phantom point collision detector (single point)
#include <Physics/Collide/Query/Collector/PointCollector/hkpAllCdPointCollector.h>			// Phantom point collision detector (all collisions)
#include <Physics/Collide/Query/Collector/BodyPairCollector/hkpAllCdBodyPairCollector.h>	// Phantom shape collision (store shapes)
#include <Physics/Collide/Query/Collector/BodyPairCollector/hkpFirstCdBodyPairCollector.h>	// Phantom shape collision (store first hit shape)
#include <Physics/Collide/Query/Collector/BodyPairCollector/hkpFlagCdBodyPairCollector.h>	// Phantom shape collision (store boolean collision)
#include <Physics/Internal/Collide/StaticCompound/hkpStaticCompoundShape.h>			// Compound shape objects
#include <Physics/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>
#include <Physics/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShapeCinfo.h>
#include <Physics/Internal/Collide/Mopp/Code/hkpMoppCode.h>
#include <Physics/Collide/Shape/Compound/Collection/CompressedMesh/hkpCompressedMeshShape.h>
#include <Physics/Collide/Shape/Compound/Collection/CompressedMesh/hkpCompressedMeshShapeBuilder.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h> 
#include <Physics/Collide/Shape/Compound/Collection/StorageExtendedMesh/hkpStorageExtendedMeshShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppCompilerInput.h>
#include <Physics/Collide/Shape/Compound/Tree/hkpBvTreeShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
#include <Physics/Collide/Shape/Compound/Collection/Mesh/hkpNamedMeshMaterial.h>

#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics/Collide/Shape/Convex/ConvexTranslate/hkpConvexTranslateShape.h>
#include <Physics/Collide/Shape/Compound/Collection/List/hkpListShape.h>

#include <Physics/Dynamics/Constraint/hkpConstraintInstance.h>
#include <Physics/Dynamics/Constraint/Bilateral/BallAndSocket/hkpBallAndSocketConstraintData.h> 
#include <Physics/Dynamics/Constraint/Bilateral/Hinge/hkpHingeConstraintData.h> 
#include <Physics/Dynamics/Constraint/Bilateral/LimitedHinge/hkpLimitedHingeConstraintData.h> 
#include <Physics/Dynamics/Constraint/Bilateral/StiffSpring/hkpStiffSpringConstraintData.h> 
#include <Physics/Dynamics/Constraint/Bilateral/Ragdoll/hkpRagdollConstraintData.h> 
#include <Physics/Dynamics/Constraint/Malleable/hkpMalleableConstraintData.h> 

#include <Physics/Utilities/Actions/Wind/hkpWind.h>
#include <Physics/Utilities/Actions/Wind/hkpWindRegion.h>

#include <Physics/Collide/Filter/hkpCollisionFilter.h> 
#include <Physics/Collide/Filter/Group/hkpGroupFilter.h> 

#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h> 
#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h> 

// Havok Animation Includes
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/Animation/Interleaved/hkaInterleavedUncompressedAnimation.h>
#include <Animation/Animation/Animation/Mirrored/hkaMirroredSkeleton.h> 
#include <Animation/Animation/Animation/Mirrored/hkaMirroredAnimation.h> 
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h> 
#include <Animation/Animation/Mapper/hkaSkeletonMapperData.h> 
#include <Animation/Animation/Mapper/hkaSkeletonMapperUtils.h> 

#define _HAVOK_VISUAL_DEBUGGER_	// Change this to a #define to gain access to the Havok Visual debugger.
#ifdef _HAVOK_VISUAL_DEBUGGER_
// Havok Debugger Includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>
#endif
*/

#include "Box2D/Box2D.h"


//===Error Reporter Function===
static void __cdecl errorReport( const char* msg, void* userContext )
{
    //using namespace std;
    //printf("%s", msg);
	debug::Console->PrintMessage( std::string(msg) + "\n" );
}

//===Physics Class Typedefs===
//typedef hkpRigidBody	physRigidBody;
//typedef hkpShape		physShape;		// Collision Shape
//typedef hkpPhantom		physCollisionVolume;

//===Rigidbody Information Struct===
/*struct physRigidBodyInfo
{
	//==Basic Properties==
	enum eMotionType
	{
		MotionDynamic,
		MotionFixed,
		MotionKeyframed
	} motionType;
	Vector3d position;
	//Quaternion rotation;
	float friction;
	float restitution;

	//==Mass Properties==
	float mass;
	Vector3d centerOfMass;
	//Vector3d inertialTensor;

	//==Collision Properties==
	physShape* shape;
	//collisionFilterInfo
	enum eQualityType
	{
		PhysicsQualityDefault			= HK_COLLIDABLE_QUALITY_INVALID,
		PhysicsQualityDebris			= HK_COLLIDABLE_QUALITY_DEBRIS,
		PhysicsQualityDebris_SimpleTOI	= HK_COLLIDABLE_QUALITY_DEBRIS_SIMPLE_TOI,
		PhysicsQualityMoving			= HK_COLLIDABLE_QUALITY_MOVING,
		PhysicsQualityCritical			= HK_COLLIDABLE_QUALITY_CRITICAL,
		PhysicsQualityBullet			= HK_COLLIDABLE_QUALITY_BULLET,
		PhysicsQualityCharacter			= HK_COLLIDABLE_QUALITY_CHARACTER
	} qualityType;
	float allowedPenetrationDepth;

	//==Damping and Clamping Properties==
	float linearDamping;
	float angularDamping;
	float maxLinearVelocity;
	float maxAngularVelocity;
};*/

/*typedef hkpRigidBodyCinfo			physRigidBodyInfo;
typedef hkpMotion::MotionType		physMotionType;
typedef hkpMotion					physMotion;
typedef hkpCollidableQualityType	physMotionQualityType;*/

//
// typdef hkpContactPointEvent physContactPointEvent;

//===============================================================================================//
// Box2D - Havok compatibility
//===============================================================================================//

// Types
//typedef int							physRigidBodyInfo;
//typedef int							physMotionType;
//typedef b2Fixture					physMotion;
//typedef int							physMotionQualityType;

class physObject {
public:
	void removeReference() {;}
};

typedef Real						physReal;

class physAabb;

// Havok interim
class hkaAnimation {};
class hkaSkeleton {};
class hkaAnimatedSkeleton {};
class hkaMirroredSkeleton {};
class hkaAnimationBinding {};
class hkReferencedObject {};

class hkpCharacterRigidBodyCinfo {};

// Classes
class physWind {};
class physWindRegion;
class physAabbPhantom : public physCollisionVolume { public: void setAabb(physAabb&){;} };
class physPhantomCallbackShape {};

class physWorldRayCastInput {};
class physRayHitCollector {};

class physCollidable {};
class physLinearCastInput {};
class physCdPointCollector {};
class physCollisionInput {};

class physEntity : public physObject {};
class physConstraintInstance {};
class physPhantom {};
class physWorldPostSimulationListener {};

class physContactPointEvent;
class physContactListener;

class physMotion;
typedef physMotion hkpMotion;

class hkpMalleableConstraintData : public physObject {
public:
	physReal m_strength;
};
class hkpConstraintInstance {};

//class physWorld {};
typedef b2World physWorld;
//typedef b2Filter physCollisionFilter;
#include "physical/physics/motion/physCollisionFilter.h"

// Class wrapping
#include "physical/wrapper/wrapper_common.h"
#include "physical/wrapper/physVector4.h"
#include "physical/wrapper/physAabb.h"
#include "physical/wrapper/physRigidBodyInfo.h"
#include "physical/wrapper/physMotion.h"
#include "physical/wrapper/physWindRegion.h"
#include "physical/wrapper/physContactEvents.h"

// Defines
//#define HK_DECLARE_CLASS_ALLOCATOR
//#define HK_MEMORY_CLASS_BASE
#define PHYS_CLASS_ALLOCATOR
#define HK_NULL NULL

#endif