

#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

// Includes
#include "core/math/Ray.h"
#include "core/math/Cubic.h"
#include "physical/physics/raycast/RaycastHit.h"
//#include "CVoxelTerrain.h"
//#include "CBoob.h"
/*#include "CCubic.h"
class CBoob;
struct subblock16;
struct subblock8;
#include "BlockInfo.h"*/
#include "core/types/ModelData.h"
//#include "physical/physics/CPhysics.h"

// Needed Typdef
typedef uint32_t r_bitmask;

class physMaterial;

// Class Definition
class CRaycaster
{
private:
	// Cannot be instanced
	//virtual void ___no_instance___ ( void ) =0;

	//static BlockTrackInfo m_blockInfo;
	//static physMaterial*	m_lastHitMaterial;
public:
	ENGINE_API static bool	Raycast	( const Ray & ray, ftype max_dist, RaycastHit * pOutHitInfo, r_bitmask collisionFilter, void* mismatch=NULL );
	//static bool	Raycast	( const Ray & ray, ftype max_dist, RaycastHit * pOutHitInfo, BlockTrackInfo * pOutBlockInfo, r_bitmask collisionFilter, void* mismatch=NULL );

	ENGINE_API static bool Linecast ( const Ray & ray, ftype max_dist, physShape* pShape, RaycastHit* pOutHitInfo, const int hitInfoArrayCount, r_bitmask collisionFilter, void* mismatch=NULL );

	//static bool	BoxCollides	( Maths::Cubic const&, r_bitmask );

	//static const BlockTrackInfo& HitBlock ( void ) { return m_blockInfo; };

	ENGINE_API static int Raycast ( const Ray & ray, ftype max_dist, const CModelData* model, Vector3d& out_point, Vector3d& out_bary );
	ENGINE_API static int RaycastExpensiveMiss ( const Ray & ray, ftype max_dist, const CModelData* model, Vector3d& out_point, Vector3d& out_bary );

public:
	ENGINE_API static physMaterial*	m_lastHitMaterial;
};

// Typdefs
ENGINE_API extern CRaycaster Raycaster;

#endif