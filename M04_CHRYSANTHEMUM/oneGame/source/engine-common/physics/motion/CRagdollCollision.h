
#ifndef _C_RAGDOLL_COLLISION_H_
#define _C_RAGDOLL_COLLISION_H_

#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "physical/physics/CPhysics.h"
#include "engine/physics/motion/CMotion.h"
//#include "CSkinnedModel.h"

class CSkinnedModel;
class CActor;

class CRagdollCollision : public CMotion
{
	ClassName( "CRagdollCollision" );
public:
	ENGCOM_API explicit CRagdollCollision ( CSkinnedModel* );
	ENGCOM_API ~CRagdollCollision ( void );

	void Update ( void ) override;
	void LateUpdate ( void ) override;
	void FixedUpdate ( void ) override;
	void RigidbodyUpdate ( void ) override;
	void PostUpdate ( void ) override;
	void PostFixedUpdate ( void ) override;

	ENGCOM_API void SetActor ( CActor* n_actor );
	ENGCOM_API CActor* GetActor ( void );
	ENGCOM_API ftype	GetMultiplier ( physRigidBody* );

private:
	void CreateJoints ( void );
	void CreateMapping ( void );

	CSkinnedModel*		m_skinnedmodel;
	CActor*				m_owning_actor;

	struct hitboxEntry {
		int				boneIndex;
		int				hitboxIndex;
		
		Vector3d		impulse;
		XTransform		start;

		arstring<64>	name;

		physRigidBody*	rigidbody;
		hkpMalleableConstraintData*	constraint;
		hkpConstraintInstance*	constraint_instance;
	};
	std::vector<hitboxEntry>	m_hitboxList;


	bool hasJoints;
	/*hkaSkeletonMapper* mMapperRagdollToAnim;
	hkaSkeletonMapper* mMapperAnimToRagdoll;
	hkaSkeleton* skeletonModel;
	hkaSkeleton* skeletonRagdoll;*/
};

#endif//_C_RAGDOLL_COLLISION_H_