
#include "CRagdollCollision.h"
//#include "Water.h"
//#include "physical/physics/water/Water.h"

#include "core-ext/animation/Skeleton.h"
#include "core-ext/types/sHitbox.h"
#include "physical/skeleton/skeletonBone.h"

//#include "renderer/logic/model/CSkinnedModel.h"

//#include "physical/physics/motion/physRigidbody.h"
//#include "physical/physics/shapes/physBoxShape.h"

#include <list>
using std::list;

CRagdollCollision::CRagdollCollision ( const prRigidbodyCreateParams& params, CSkinnedModel* sourceModel )
	: CMotion(params.owner, params.ownerType)
{
	hasJoints = false;
	m_skinnedmodel = sourceModel;

	// Set the layer
	layer = physical::layer::Hitboxes;

	hitboxEntry hb;
	std::vector<sHitbox>* mdl_hitboxes = m_skinnedmodel->GetHitboxes();
	animation::Skeleton* skeleton = m_skinnedmodel->GetSkeleton();

	// Create the model/world space of the reference pose
	std::vector<core::TransformLite> pose_model;
	pose_model.resize( skeleton->parent.size() );
	core::TransformUtility::LocalToWorld( &skeleton->parent[0], &skeleton->reference_xpose[0], &pose_model[0], pose_model.size() );

#if 0
	// Add root as first hitbox/bone
	{
		//glBone* targetBone = m_skinnedmodel->GetSkeletonList()->at(0);

		physShape* tCollider;
		//tCollider = Physics::CreateBoxShape( Vector3f(0.17f,0.17f,0.17f) );
		tCollider = new physBoxShape( Vector3f(0.17f,0.17f,0.17f) );

		physRigidBodyInfo tInfo;
		tInfo.m_mass = 3.0f;
		tInfo.m_shape = tCollider;
		tInfo.m_centerOfMass = physVector4( 0,0,0 );
		tInfo.m_friction = 0.5f;
		tInfo.m_motionType = physMotion::MOTION_DYNAMIC;
		//Vector3f targetPosition = targetBone->xRagdollPoseModel.position;//targetBone->transform.position;
		Vector3f targetPosition = pose_model[0].world.position;
		tInfo.m_position = physVector4( targetPosition.x, targetPosition.y, targetPosition.z );
		//Quaternion targetRotation = targetBone->xRagdollPoseModel.rotation;//targetBone->transform.rotation.getQuaternion();
		Quaternion targetRotation = pose_model[0].world.rotation;
		tInfo.m_rotation = physQuaternion( targetRotation.x, targetRotation.y, targetRotation.z, targetRotation.w );
		tInfo.m_linearDamping = 0.1f;
		tInfo.m_angularDamping = 0.2f;

		tInfo.m_collisionFilterInfo = CPhysics::GetCollisionFilter( Layers::PhysicsTypes::PHYS_HITBOX );
			
		hitboxEntry thb;
		thb.rigidbody = new physRigidBody( &tInfo );
		thb.rigidbody->setUserData( GetId() ); // Give the rigidbody this ragoll's ID.

		thb.impulse = Vector3f(0,0,0);
		thb.boneIndex = 0;//targetBone->index;
		thb.hitboxIndex = 0;
		thb.name = "Root";
		thb.constraint = NULL;

		m_hitboxList.push_back( thb );
		delete_safe(tCollider);
	}
	// Loop through model hitboxes
	for ( uint i = 0; i < mdl_hitboxes->size(); ++i ) 
	{
		//glBone* targetBone = m_skinnedmodel->GetSkeletonList()->at(mdl_hitboxes->at(i).indexLink);
		int boneIndex = mdl_hitboxes->at(i).indexLink;

		physShape* tCollider;
		//tCollider = Physics::CreateBoxShape( mdl_hitboxes->at(i).extents*1.3f, mdl_hitboxes->at(i).center );
		tCollider = new physBoxShape( mdl_hitboxes->at(i).extents*1.3f, mdl_hitboxes->at(i).center );

		physRigidBodyInfo tInfo;
		tInfo.m_mass = 3.0f;
		tInfo.m_shape = tCollider;
		tInfo.m_centerOfMass = physVector4( 0,0,0 );
		tInfo.m_friction = 0.5f;
		tInfo.m_motionType = physMotion::MOTION_DYNAMIC;
		//Vector3f targetPosition = targetBone->xRagdollPoseModel.position;//targetBone->transform.position;
		Vector3f targetPosition = pose_model[boneIndex].world.position;
		tInfo.m_position = physVector4( targetPosition.x, targetPosition.y, targetPosition.z );
		//Quaternion targetRotation = targetBone->xRagdollPoseModel.rotation;// targetBone->transform.rotation.getQuaternion();
		Quaternion targetRotation = pose_model[boneIndex].world.rotation;
		tInfo.m_rotation = physQuaternion( targetRotation.x, targetRotation.y, targetRotation.z, targetRotation.w );
		tInfo.m_linearDamping = 0.1f;
		tInfo.m_angularDamping = 0.3f;

		tInfo.m_collisionFilterInfo = CPhysics::GetCollisionFilter( Layers::PhysicsTypes::PHYS_HITBOX );

		hitboxEntry thb;
		thb.rigidbody = new physRigidBody( &tInfo );
		thb.rigidbody->setUserData( GetId() ); // Give the rigidbody this ragoll's ID.

		thb.impulse = Vector3f(0,0,0);
		thb.boneIndex = boneIndex;
		thb.hitboxIndex = m_hitboxList.size();
		thb.name = mdl_hitboxes->at(i).name;
		thb.constraint = NULL;

		m_hitboxList.push_back( thb );
		delete_safe(tCollider);
	}
#endif

	CreateJoints(pose_model);
	CreateMapping(pose_model);

	// Now, change inertias so ragdolls stop exploding
	/*{
		std::vector<hkpConstraintInstance*> constraints;
		for ( auto hb = m_hitboxList.begin(); hb != m_hitboxList.end(); ++hb )
		{
			if ( hb->constraint_instance ) {
				constraints.push_back( hb->constraint_instance );
			}
		}
		hkpInertiaTensorComputer::optimizeInertiasOfConstraintTree( &(constraints[0]), constraints.size(), m_hitboxList[0].rigidbody, 4 );
	}*/
}

void CRagdollCollision::CreateJoints ( std::vector<core::TransformLite>& pose_model )
{
	std::vector<sHitbox>* mdl_hitboxes = m_skinnedmodel->GetHitboxes();
	animation::Skeleton* skeleton = m_skinnedmodel->GetSkeleton();

	// Loop through model skeleton, child-first order, to create joints
	//Transform* tr_root = m_skinnedmodel->GetSkeletonRoot();
	//std::list<Transform*> tr_bonelist;
	//std::list<Transform*> tr_heirarchylist;
	//tr_bonelist.push_back( tr_root );
	//// First add all the children to the list in child-first order
	//while ( !tr_bonelist.empty() ) {
	//	Transform* tr_current = tr_bonelist.front();
	//	tr_bonelist.pop_front();
	//	tr_heirarchylist.push_front( tr_current ); // Add in child-first order
	//	for ( uint i = 0; i < tr_current->children.size(); ++i )
	//	{
	//		tr_bonelist.push_back( tr_current->children[i] );
	//	}
	//}
	//// Loop through the children and generate joints as needed
	///*while ( !tr_heirarchylist.empty() ) {
	//	Transform* tr_current = tr_heirarchylist.front();
	//	tr_heirarchylist.pop_front();
	//	// Skip root
	//	if ( tr_current == m_skinnedmodel->GetSkeletonRoot() ) {
	//		continue;
	//	}
	//	glBone* targetBone = (glBone*)tr_current->pOwnerRenderer;
	//	// Search the hitboxes for this bone
	//	uint hitboxIndex = uint(-1);
	//	for ( uint i = 0; i < m_hitboxList.size(); ++i ) 
	//	{
	//		if ( m_hitboxList[i].boneIndex == targetBone->index ) {
	//			hitboxIndex = i; // Save hitbox index
	//			// Break out of loop
	//			i = m_hitboxList.size() + 1;
	//		}
	//	}
	//	// Skip if no hitbox
	//	if ( hitboxIndex == uint(-1) ) {
	//		continue;
	//	}*/
#if 0
	for ( uint i = 0; i < m_hitboxList.size(); ++i )
	{
		//Transform* tr_current = &(m_skinnedmodel->GetSkeletonList()->at(mdl_hitboxes->at(i).indexLink)->transform);
		//Transform* tr_current = &(m_skinnedmodel->GetSkeletonList()->at(m_hitboxList[i].boneIndex)->transform);
		uint hitboxIndex = i;
		//glBone* targetBone = (glBone*)tr_current->owner;
		int boneIndex = m_hitboxList[i].boneIndex;

		// Has a hitbox, so find the next parent with a hitbox (search up the list)
		int parentHitboxIndex = -1;
		// Search up the parents
		//CTransform* tr_next = tr_current->GetParent();
		int tr_next = skeleton->parent[boneIndex];
		//glBone* parentBone;
		int parentBone;
		//while ( tr_next != &CTransform::root )
		while ( tr_next >= 0 )
		{
			//parentBone = (glBone*)tr_next->owner;
			parentBone = tr_next;
			for ( uint i = 0; i < m_hitboxList.size(); ++i ) 
			{
				if ( m_hitboxList[i].boneIndex == tr_next )
				{
					parentHitboxIndex = i; // Save hitbox index
					// Break out of loop
					i = m_hitboxList.size() + 1;
				}
			}
			if ( parentHitboxIndex != -1 )
			{
				break;
			}
			//tr_next = tr_next->GetParent();
			 tr_next = skeleton->parent[tr_next];
		}
		// Skip if no parent hitbox
		if ( parentHitboxIndex == -1 )
		{
			m_hitboxList[i].constraint = NULL;
			m_hitboxList[i].constraint_instance = NULL;
			continue;
		}
		//cout << "PARENT HITBOX: " << hitboxIndex << " to " << parentHitboxIndex << endl;
		// Have a parent, so make a joint between the two rigidbodies at the position of the current bone
		/*{
			hkpConstraintData* t_cst_data;
			hkpConstraintInstance* t_cst_inst;
			if ( (targetBone->name.find( "Calf" ) != string::npos) || (targetBone->name.find( "Forearm" ) != string::npos) )
			{
				//targetBone->transform.Up();
				Vector3f axisUp = targetBone->xRagdollPoseModel.rotation * Vector3f::up;

				hkpLimitedHingeConstraintData* hc;
				hc = new hkpLimitedHingeConstraintData();
				hc->setInWorldSpace(
					m_hitboxList[hitboxIndex].rigidbody->getTransform(),
					m_hitboxList[parentHitboxIndex].rigidbody->getTransform(),
					m_hitboxList[hitboxIndex].rigidbody->getPosition(),
					hkVector4( axisUp.x, axisUp.y, axisUp.z ) );
				hc->setMinAngularLimit( -HK_REAL_PI * 0.9f );
				hc->setMaxAngularLimit( HK_REAL_PI * 0.1f );

				t_cst_data = hc;
			}
			else if ( (m_hitboxList[i].name.compare( "Hips" )) )
			{
				hkpBallAndSocketConstraintData* bs;
				bs = new hkpBallAndSocketConstraintData(); 
				bs->setInWorldSpace(
					m_hitboxList[hitboxIndex].rigidbody->getTransform(),
					m_hitboxList[parentHitboxIndex].rigidbody->getTransform(),
					m_hitboxList[hitboxIndex].rigidbody->getPosition() );

				t_cst_data = bs;
			}
			else
			{
				Vector3f axisUp = targetBone->xRagdollPoseModel.rotation * Vector3f::up;
				Vector3f axisSide = targetBone->xRagdollPoseModel.rotation * Vector3f::left;
				Vector3f axisForward = targetBone->xRagdollPoseModel.rotation * Vector3f::forward;

				hkpRagdollConstraintData* rdc;
				
				rdc = new hkpRagdollConstraintData();

				hkVector4 twistAxisA( axisForward.x, axisForward.y, axisForward.z );
				hkVector4 planeAxisA( axisUp.x, axisUp.y, axisUp.z );

				rdc->setInWorldSpace(
					m_hitboxList[hitboxIndex].rigidbody->getTransform(),
					m_hitboxList[parentHitboxIndex].rigidbody->getTransform(),
					m_hitboxList[hitboxIndex].rigidbody->getPosition(),
					twistAxisA, planeAxisA );

				rdc->setTwistMinAngularLimit( -HK_REAL_PI * 0.45f );
				rdc->setTwistMaxAngularLimit( +HK_REAL_PI * 0.45f );
				rdc->setPlaneMinAngularLimit( -HK_REAL_PI * 0.45f );
				rdc->setPlaneMaxAngularLimit( +HK_REAL_PI * 0.45f );

				rdc->setAsymmetricConeAngle( -HK_REAL_PI * 0.45f, +HK_REAL_PI * 0.45f );

				t_cst_data = rdc;
			}

			// Wrap constraint in malleable constraint data
			m_hitboxList[i].constraint = new hkpMalleableConstraintData ( t_cst_data );
			m_hitboxList[i].constraint->m_strength = 1;

			t_cst_inst = new hkpConstraintInstance(
				m_hitboxList[hitboxIndex].rigidbody,
				m_hitboxList[parentHitboxIndex].rigidbody,
				m_hitboxList[i].constraint);

			CPhysics::AddConstraint( t_cst_inst );

			m_hitboxList[i].constraint_instance = t_cst_inst;

			t_cst_data->removeReference();  
			t_cst_inst->removeReference();
		}*/
	}
#endif
}

//#include <boost/algorithm/string.hpp>
#include "core/utils/string.h"
int boneNameCompare ( const char * bone1, const char * bone2 )
{
	string sbone1 (bone1);
	string sbone2 (bone2);
	sbone1 = core::utils::string::GetLower(sbone1);
	sbone2 = core::utils::string::GetLower(sbone2);
	if ( sbone1.find(sbone2) != string::npos ) {
		return 0;
	}
	else if ( sbone2.find(sbone1) != string::npos ) {
		return 0;
	}
	else {
		return sbone1.compare(sbone2);
	}
}

void CRagdollCollision::CreateMapping ( std::vector<core::TransformLite>& pose_model )
{
	// First, create the skeletons
	/*skeletonModel = new hkaSkeleton();
	skeletonRagdoll = new hkaSkeleton();

	// Loop through model bones and create the skeleton
	skeletonModel->m_referencePose.setSize( m_skinnedmodel->GetSkeletonList()->size() );
	skeletonModel->m_parentIndices.setSize( m_skinnedmodel->GetSkeletonList()->size() );
	skeletonModel->m_bones.setSize( m_skinnedmodel->GetSkeletonList()->size() );
	XTransform* temp;
	for ( int i = 0; i < skeletonModel->m_bones.getSize(); ++i )
	{
		glBone* currentBone = m_skinnedmodel->GetSkeletonList()->at(i);
		temp = &(currentBone->xBindPose);
		skeletonModel->m_referencePose[i] = hkQsTransform(
			hkVector4( temp->position.x, temp->position.y, temp->position.z ),
			hkQuaternion( temp->rotation.x, temp->rotation.y, temp->rotation.z, temp->rotation.w ),
			hkVector4( temp->scale.x, temp->scale.y, temp->scale.z ) );
		skeletonModel->m_bones[i].m_name = currentBone->name.c_str();
		skeletonModel->m_bones[i].m_lockTranslation = false;

		// Search for the parent index
		skeletonModel->m_parentIndices[i] = -1;
		for ( int j = 0; j < skeletonModel->m_bones.getSize(); ++j ) {
			glBone* parentBone = m_skinnedmodel->GetSkeletonList()->at(j);
			if ( currentBone->transform.GetParent() == (&(parentBone->transform)) ) {
				skeletonModel->m_parentIndices[i] = j;
				j = skeletonModel->m_bones.getSize();
			}
		}
	}

	// Loop through hitboxes and create another skeleton
	skeletonRagdoll->m_referencePose.setSize( m_hitboxList.size() );
	skeletonRagdoll->m_parentIndices.setSize( m_hitboxList.size() );
	skeletonRagdoll->m_bones.setSize( m_hitboxList.size() );
	for ( int i = 0; i < m_hitboxList.size(); ++i )
	{
		glBone* currentBone = m_skinnedmodel->GetSkeletonList()->at(m_hitboxList[i].boneIndex);
		temp = &(currentBone->xBindPose);
		skeletonRagdoll->m_referencePose[i] = hkQsTransform(
			hkVector4( temp->position.x, temp->position.y, temp->position.z ),
			hkQuaternion( temp->rotation.x, temp->rotation.y, temp->rotation.z, temp->rotation.w ),
			hkVector4( temp->scale.x, temp->scale.y, temp->scale.z ) );
		skeletonRagdoll->m_bones[i].m_name = currentBone->name.c_str();
		skeletonRagdoll->m_bones[i].m_lockTranslation = false;

		// Search up the parents
		CTransform* tr_next = currentBone->transform.GetParent();
		skeletonRagdoll->m_parentIndices[i] = -1;
		while ( tr_next != &CTransform::root ) {
			glBone* parentBone = (glBone*)tr_next->pOwnerRenderer;
			for ( uint j = 0; j < m_hitboxList.size(); ++j ) 
			{
				if ( m_hitboxList[j].boneIndex == parentBone->index ) {
					skeletonRagdoll->m_parentIndices[i] = j; // Save hitbox index
		cout << "PARENT HITBOX: " << i << " to " << j << endl;
					// Break out of loop
					j = m_hitboxList.size() + 1;
				}
			}
			if ( skeletonRagdoll->m_parentIndices[i] != -1 ) {
				break;
			}
			tr_next = tr_next->GetParent();
		}
	}

	///
	hkaSkeletonMapperUtils::Params param;
	param.m_skeletonA = skeletonModel;
	param.m_skeletonB = skeletonRagdoll;
	param.m_autodetectChains = false;
	param.m_compareNames = boneNameCompare;
	hkaSkeletonMapperUtils::UserChain userchain;
	userchain.m_start = "Spine";
	userchain.m_end = "Spine3";
	param.m_userChains.pushBack( userchain );

	hkaSkeletonMapperData* mdRagdollToAnim = new hkaSkeletonMapperData();
	hkaSkeletonMapperData* mdAnimToRagdoll = new hkaSkeletonMapperData();
	hkaSkeletonMapperUtils::createMapping( param, *mdAnimToRagdoll, *mdRagdollToAnim );

	mMapperRagdollToAnim = new hkaSkeletonMapper( *mdRagdollToAnim );
	mMapperAnimToRagdoll = new hkaSkeletonMapper( *mdAnimToRagdoll );
	*/
}

CRagdollCollision::~CRagdollCollision ( void )
{
	//for ( auto hb = m_hitboxList.begin(); hb != m_hitboxList.end(); ++hb ) {
	//	//Physics::FreeRigidBody( hb->rigidbody );
	//	delete_safe( hb->rigidbody );
	//	if ( hb->constraint ) {
	//		Physics::RemoveReference(hb->constraint);
	//		hb->constraint = NULL;
	//	}
	//}
	//m_hitboxList.clear();
}

void CRagdollCollision::Update ( void )
{
	//bool inWater = WaterTester::Get()->PositionInside( m_hitboxList[0].start.position );
	//auto skeleton = m_skinnedmodel->GetSkeleton();
	//for ( auto hb = m_hitboxList.begin(); hb != m_hitboxList.end(); ++hb )
	//{
	//	//glBone* targetBone = m_skinnedmodel->GetSkeletonList()->at(hb->boneIndex);
	//	if ( hb->constraint )
	//	{
	//		//glBone* parentBone = (glBone*)(targetBone->transform.owner);
	//		//hb->constraint->m_strength = std::max<Real>(targetBone->ragdollStrength,parentBone->ragdollStrength); //* 0.7f;
	//		if ( skeleton->parent[hb->boneIndex] >= 0 )
	//		{
	//			hb->constraint->m_strength = std::max<Real>(
	//				skeleton->ext_physics_strength[hb->boneIndex],
	//				skeleton->ext_physics_strength[skeleton->parent[hb->boneIndex]]);
	//		}
	//	}
	//	if ( hb->rigidbody )
	//	{
	//		if ( !inWater ) {
	//			//hb->rigidbody->setGravityFactor( targetBone->ragdollStrength );
	//			hb->rigidbody->setGravityFactor( skeleton->ext_physics_strength[hb->boneIndex] );
	//			hb->rigidbody->setLinearDamping( 0.1f );
	//		}
	//		else {
	//			hb->rigidbody->setGravityFactor( 0.05f );
	//			hb->rigidbody->setLinearDamping( 0.6f );
	//		}
	//	}
	//}
}

void CRagdollCollision::LateUpdate ( void )
{
	//// Set the current hitbox rigidbody position, so can create proper velocities later
	//for ( auto hb = m_hitboxList.begin(); hb != m_hitboxList.end(); ++hb )
	//{
	//	hb->start.position = hb->rigidbody->getPosition();
	//	hb->start.rotation = hb->rigidbody->getRotation();
	//}
}

void CRagdollCollision::PostUpdate ( void )
{
	
}

void CRagdollCollision::PostFixedUpdate ( void )
{

}

void CRagdollCollision::RigidbodyUpdate ( Real interpolation )
{
	animation::Skeleton* skeleton = m_skinnedmodel->GetSkeleton();
	// Need a Ragdoll to Animation conversion here
	for ( auto hb = m_hitboxList.begin(); hb != m_hitboxList.end(); ++hb )
	{	// TODO: apply impulse and continue
		hb->impulse = Vector3f(0,0,0);

		// If the ragdoll strength is high with this one, take the output from the physics
		/*glBone* targetBone = m_skinnedmodel->GetSkeletonList()->at(hb->boneIndex);
		if ( targetBone->ragdollStrength > FTYPE_PRECISION )
		{
			Vector3f nextPosition = hb->rigidbody->getPosition();
			nextPosition -= m_skinnedmodel->transform.position;
			nextPosition = m_skinnedmodel->transform.rotation.inverse() * nextPosition;
			targetBone->transform.position = targetBone->transform.position.lerp(nextPosition,targetBone->ragdollStrength);

			Quaternion nextRotation = hb->rigidbody->getRotation();
			Rotator nextRotator = m_skinnedmodel->transform.rotation.inverse() * Rotator(nextRotation);
			targetBone->transform.rotation = targetBone->transform.rotation.LerpTo(nextRotator,targetBone->ragdollStrength);
		}*/
		if ( skeleton->ext_physics_strength[hb->boneIndex] > FTYPE_PRECISION)
		{
			throw core::NotYetImplementedException();
		}
	}
}


void CRagdollCollision::FixedUpdate ( void )
{
	animation::Skeleton* skeleton = m_skinnedmodel->GetSkeleton();
	// TODO: Ensure lite transform is updated
#if 0 

	// Need a Animation to Ragdoll conversion here
	for ( auto hb = m_hitboxList.begin(); hb != m_hitboxList.end(); ++hb )
	{
		//glBone* targetBone = m_skinnedmodel->GetSkeletonList()->at(hb->boneIndex);
		core::TransformLite* bone_transform = &skeleton->current_transform[hb->boneIndex]; 
		Real bone_strength = skeleton->ext_physics_strength[hb->boneIndex];

		Vector3f targetPosition = m_skinnedmodel->transform.rotation * bone_transform->world.position + m_skinnedmodel->transform.position;
		Quaternion targetRotation = ( m_skinnedmodel->transform.rotation * bone_transform->world.rotation ).getQuaternion();

		Vector3f linearVelocity = hb->rigidbody->getLinearVelocity();
		Vector4f angularVelocity = hb->rigidbody->getAngularVelocity();
		linearVelocity *= bone_strength;
		angularVelocity *= bone_strength;
		Vector3f currentPosition = hb->rigidbody->getPosition();
		Vector3f deltaPosition = targetPosition - currentPosition;
		//currentPosition.setInterpolate( hkVector4(targetPosition.x,targetPosition.y,targetPosition.z), currentPosition, targetBone->ragdollStrength );
		currentPosition = targetPosition.lerp( currentPosition, bone_strength );
		Quaternion currentRotation = hb->rigidbody->getRotation();
		Quaternion deltaRotation = targetRotation;
		Quaternion invRotation = currentRotation;
//		invRotation.setInverse( currentRotation );
		invRotation.Invert();
		//deltaRotation.mul( invRotation );
		deltaRotation = deltaRotation * invRotation;
		//currentRotation.setSlerp( hkQuaternion(targetRotation.x,targetRotation.y,targetRotation.z,targetRotation.w), currentRotation, targetBone->ragdollStrength );
		currentRotation = targetRotation.Slerp( currentRotation, bone_strength );

		// now, motorize the rigidbody
		deltaPosition = targetPosition;//hkVector4( targetPosition.x, targetPosition.y, targetPosition.z );
		//deltaPosition.sub( hkVector4( hb->start.position.x,hb->start.position.y,hb->start.position.z ) );
		deltaPosition -= hb->start.position;

		//deltaPosition.mul( (1 - targetBone->ragdollStrength)*30 );
		deltaPosition *= (1.0F - bone_strength)*30.0F;
		linearVelocity += deltaPosition;
		//deltaRotation.setSlerp( deltaRotation, hkQuaternion::getIdentity(), targetBone->ragdollStrength );
		deltaRotation = deltaRotation.Slerp( Quaternion(), bone_strength );
		angularVelocity += Vector4f( &deltaRotation.x );
		
		// set velocities and position
		hb->rigidbody->setLinearVelocity( linearVelocity );
		hb->rigidbody->setAngularVelocity( angularVelocity );
		hb->rigidbody->setPositionAndRotation( currentPosition, currentRotation );

	}
#endif
	/*hkaPose modelPose ( skeletonModel );
	hkaPose ragdollPose ( skeletonRagdoll );

	// Get current anim pose
	hkArray<hkQsTransform>& tempTransforms = modelPose.accessUnsyncedPoseModelSpace();
	for ( uint i = 0; i < skeletonModel->m_bones.getSize(); ++i )
	{
		glBone* bone = m_skinnedmodel->GetSkeletonList()->at(i);
		Vector3f position = bone->transform.position;
		tempTransforms[i].m_translation = hkVector4( position.x, position.y, position.z );
		Quaternion rotation = bone->transform.rotation;
		tempTransforms[i].m_rotation = hkQuaternion( rotation.x, rotation.y, rotation.z, rotation.w );
		Quaternion scale = bone->transform.scale;
		tempTransforms[i].m_scale = hkVector4( scale.x, scale.y, scale.z );
	}

	ragdollPose.setToReferencePose();

	mMapperAnimToRagdoll->mapPose( modelPose, ragdollPose, hkaSkeletonMapper::CURRENT_POSE );
	ragdollPose.syncModelSpace();

	// Apply the animation to the ragdoll
	for ( auto hb = m_hitboxList.begin(); hb != m_hitboxList.end(); ++hb )
	{
		glBone* targetBone = m_skinnedmodel->GetSkeletonList()->at(hb->boneIndex);
		hkQsTransform targetTransform = ragdollPose.getBoneModelSpace(hb->hitboxIndex);

		hkVector4 linearVelocity = hb->rigidbody->getLinearVelocity();
		hkVector4 angularVelocity = hb->rigidbody->getAngularVelocity();
		linearVelocity.mul( targetBone->ragdollStrength );
		angularVelocity.mul( targetBone->ragdollStrength );

		hkVector4 currentPosition = hb->rigidbody->getPosition();
		currentPosition.setInterpolate( targetTransform.getTranslation(), currentPosition, targetBone->ragdollStrength );
		hkQuaternion currentRotation = hb->rigidbody->getRotation();
		currentRotation.setSlerp( targetTransform.getRotation(), currentRotation, targetBone->ragdollStrength );

		hb->rigidbody->setLinearVelocity( linearVelocity );
		hb->rigidbody->setAngularVelocity( angularVelocity );
		hb->rigidbody->setPositionAndRotation( currentPosition, currentRotation );
	}*/
}

void CRagdollCollision::SetActor ( CActor* n_actor )
{
	m_owning_actor = n_actor;
	owner = (CGameBehavior*)n_actor;
}

CActor* CRagdollCollision::GetActor ( void )
{
	return m_owning_actor;
}
Real	CRagdollCollision::GetMultiplier ( btRigidBody* n_hitBody )
{
	// Todo: give certain rigidbodies different damage boosters
	return 1;
}