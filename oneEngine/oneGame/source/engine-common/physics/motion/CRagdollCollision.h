//===============================================================================================//
// This will need to be reworked eventually, but since there is no pressing need for this feature
// the functionality will simply be removed.
//
// This will eventually be re-implemented as a animation & renderer dependant physics system.
//===============================================================================================//
#ifndef ENGINE_COMMON_RAGDOLL_COLLISION_H_
#define ENGINE_COMMON_RAGDOLL_COLLISION_H_

#include "core/containers/arstring.h"
#include "core-ext/transform/TransformUtility.h"
#include "engine/behavior/CGameBehavior.h"
//#include "physical/physics/CPhysics.h"
#include "engine/physics/motion/CMotion.h"
#include "engine/physics/motion/CRigidbody.h"
//#include "CSkinnedModel.h"

class RrCModel;
class CActor;
class btRigidBody;

class CRagdollCollision : public CMotion
{
	ClassName( "CRagdollCollision" );
public:
	ENGCOM_API explicit CRagdollCollision ( const prRigidbodyCreateParams& params, RrCModel* sourceModel );
	ENGCOM_API ~CRagdollCollision ( void );

	void Update ( void ) override;
	void LateUpdate ( void ) override;
	void FixedUpdate ( void ) override;
	void RigidbodyUpdate ( Real interpolation ) override;
	void PostUpdate ( void ) override;
	void PostFixedUpdate ( void ) override;

	ENGCOM_API void		SetActor ( CActor* n_actor );
	ENGCOM_API CActor*	GetActor ( void );
	ENGCOM_API Real	GetMultiplier ( btRigidBody* n_hitBody );

private:
	void CreateJoints ( std::vector<core::TransformLite>& pose_model );
	void CreateMapping ( std::vector<core::TransformLite>& pose_model );

	RrCModel*			m_model;
	CActor*				m_owning_actor;

	struct hitboxEntry 
	{
		int				boneIndex;
		int				hitboxIndex;
		
		Vector3f		impulse;
		XTransform		start;

		arstring<64>	name;

		btRigidBody*	rigidbody;
		//hkpMalleableConstraintData*	constraint;
		//hkpConstraintInstance*	constraint_instance;
	};
	std::vector<hitboxEntry>	m_hitboxList;


	bool hasJoints;
	/*hkaSkeletonMapper* mMapperRagdollToAnim;
	hkaSkeletonMapper* mMapperAnimToRagdoll;
	hkaSkeleton* skeletonModel;
	hkaSkeleton* skeletonRagdoll;*/
};

#endif//ENGINE_COMMON_RAGDOLL_COLLISION_H_