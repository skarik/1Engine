
#include "CHKAnimation.h"
#include "set/CHKAnimationSet.h"

#include "Animation/Animation/Rig/hkaSkeletonUtils.h"
#include "Animation/Animation/Ik/TwoJoints/hkaTwoJointsIkSolver.h"

#include "Animation/Animation/Ik/LookAt/hkaLookAtIkSolver.h"
#include "Animation/Animation/Ik/FootPlacement/hkaFootPlacementIkSolver.h"

//#include "CModel.h"
//#include "core/time/time.h"
#include "physical/skeleton/skeletonBone.h"
#include "physical/physics/CPhysics.h"

//#include "CDebugDrawer.h"

Vector3d CHKAnimation::defaultJointVector (0,0,-1);

// class hkWorldCaster
// Class used by the IK to check collisions against the world.
class hkWorldCaster : public hkaRaycastInterface 
{
public:
	virtual hkBool  castRay (const hkVector4 &fromWS, const hkVector4 &toWS, hkReal &hitFractionOut, hkVector4 &normalWSOut) override
	{
		return castRay( fromWS,toWS, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ), hitFractionOut,normalWSOut );
	}
	virtual hkBool  castRay (const hkVector4 &fromWS, const hkVector4 &toWS, hkUint32 collisionFilterInfo, hkReal &hitFractionOut, hkVector4 &normalWSOut) override
	{
		// Create the raycast information.
		hkpWorldRayCastInput hkInputCastRay;
		// Set the start position of the ray
		hkInputCastRay.m_from = fromWS;
		// Set the end position of the ray
		hkInputCastRay.m_to = toWS;
		// Set the ray's collision mask
		hkInputCastRay.m_filterInfo = collisionFilterInfo;

		// Create a ray hit accumulation object
		{
			hkpClosestRayHitCollector hkRayHitCollection;

			// Cast a ray into the lovely world
			//CPhysics::World()->castRay ( hkInputCastRay, hkRayHitCollection );
			//CPhysics::Raycast ( hkInputCastRay, hkRayHitCollection );
			Physics::INTERNAL_Raycast( hkInputCastRay, hkRayHitCollection );

			// If the collection has a hit recorded, then we want to grab the hit info.
			// Otherwise, we want to set the outHitInfo to report no hit.
			if ( hkRayHitCollection.hasHit() )
			{
				// Grab the hit info
				hkpWorldRayCastOutput hkRayHitOutput = hkRayHitCollection.getHit();
				hitFractionOut	= hkRayHitOutput.m_hitFraction;
				normalWSOut		= hkRayHitOutput.m_normal;
				return true;
			}
		}
		return false;
	}
};

// class animLerper
// Utility class used to blend a value "smoothly" to another value
class animLerper
{
public:
	explicit animLerper ( const Real deltaTime ) : delta(deltaTime)
	{
		;
	}
	void operator() ( Real& io_value, const Real& n_target )
	{
		io_value += ( n_target - io_value ) *  std::min<Real>( 0.5f*delta*40.0f, 0.97f );
	}
private:
	Real delta;
};

// Sets the active skeleton for this animation instance
void CHKAnimation::SetSkeleton( skeletonBone_t* rootBone, std::vector<skeletonBone_t*> &vModelSkelly )
{
	// First, create the skeleton

	mSkelly = new hkaSkeleton;
	//mSkelly->m_referenceCount = vModelSkelly.size();
	mSkelly->m_referencePose.setSize( vModelSkelly.size() );
	mSkelly->m_parentIndices.setSize( vModelSkelly.size() );
	mSkelly->m_bones.setSize( vModelSkelly.size() );

	XTransform* temp;
	for ( uint i = 0; i < vModelSkelly.size(); ++i )
	{
		temp = &(vModelSkelly[i]->xBindPose);
		mSkelly->m_referencePose[i] = hkQsTransform(
			hkVector4( temp->position.x, temp->position.y, temp->position.z ),
			hkQuaternion( temp->rotation.x, temp->rotation.y, temp->rotation.z, temp->rotation.w ),
			hkVector4( temp->scale.x, temp->scale.y, temp->scale.z ) );

		mSkelly->m_bones[i].m_name = vModelSkelly[i]->name.c_str();
		//cout << "\"" << mSkelly->m_bones[i].m_name << "\"" << endl;
		mSkelly->m_bones[i].m_lockTranslation = false;

		// Search for the parent index
		mSkelly->m_parentIndices[i] = -1;
		for ( uint j = 0; j < vModelSkelly.size(); ++j ) {
			if ( vModelSkelly[i]->transform.GetParent() == (&(vModelSkelly[j]->transform)) ) {
				mSkelly->m_parentIndices[i] = j;
				j = vModelSkelly.size();
			}
		}
	}
	//mAnimSkelly = new hkaAnimatedSkeleton( mSkelly );
}

// Returns the ragdoll pose int vModelSkelly
void CHKAnimation::GetRagdollPose( skeletonBone_t* rootBone, std::vector<skeletonBone_t*> &vModelSkelly )
{
	hkArray<hkQsTransform> nextTransforms;
	nextTransforms.setSize( mSkelly->m_bones.getSize() );
	hkArray<hkReal> tempReals;
	tempReals.setSize( mSkelly->m_bones.getSize() );

	hkaAnimation* mhkAnim = ((CHKAnimationSet*)pAnimationSet)->GetHKAnimation()->at(0);
	mhkAnim->sampleTracks( 0.0f, nextTransforms.begin(), tempReals.begin() );

	// set the ragdoll pose : D
	for ( int i = 0; i < nextTransforms.getSize(); ++i )
	{
		XTransform localTransform;
		localTransform.position = Vector3d(
			nextTransforms[i].m_translation.getComponent<0>(),
			nextTransforms[i].m_translation.getComponent<1>(),
			nextTransforms[i].m_translation.getComponent<2>() );
		localTransform.rotation = Quaternion(
			nextTransforms[i].m_rotation.m_vec.getComponent<0>(),
			nextTransforms[i].m_rotation.m_vec.getComponent<1>(),
			nextTransforms[i].m_rotation.m_vec.getComponent<2>(),
			nextTransforms[i].m_rotation.m_vec.getComponent<3>() );
		localTransform.scale = Vector3d(
			nextTransforms[i].m_scale.getComponent<0>(),
			nextTransforms[i].m_scale.getComponent<1>(),
			nextTransforms[i].m_scale.getComponent<2>() );
		vModelSkelly[i]->animTransform = localTransform;
	}
	for ( int i = 0; i < nextTransforms.getSize(); ++i )
	{
		vModelSkelly[i]->SendTransformation();
		vModelSkelly[i]->transform.LateUpdate();
		vModelSkelly[i]->xRagdollPoseModel = XTransform(
			vModelSkelly[i]->transform.position, vModelSkelly[i]->transform.rotation, vModelSkelly[i]->transform.scale );
	}
}

// Cleanup
CHKAnimation::~CHKAnimation ( void )
{
	//delete mSkelly;
	mSkelly->removeReference();
	//mAnimSkelly->removeReference();
	if ( mMirrorSkelly ) {
		mMirrorSkelly->removeReference();
	}
	/*if ( mMirrorAnim ) {
		mMirrorAnim->removeReference();
	}*/
	for ( uint i = 0; i < mMirrorAnims.size(); ++i )
	{
		mMirrorAnims.at(i)->removeReference();
	}
}

void CHKAnimation::Update ( const Real deltaTime )
{
	// Create binding
	if ( mAnimBinding == NULL ) {
		mAnimBinding = new hkaAnimationBinding();
		mAnimBinding->m_animation = ((CHKAnimationSet*)pAnimationSet)->GetHKAnimation()->at(0);
		for ( hkInt16 i = 0; i < mSkelly->m_bones.getSize(); ++i ) {
			mAnimBinding->m_transformTrackToBoneIndices.pushBack(i);
			mAnimBinding->m_floatTrackToFloatSlotIndices.pushBack(i);
		}
	}
	// Setup mirrored animations
	//SetupMirrorMode(); 

	bool bFirstAnim = true;
	bool bFirstAimr = true;
	bool bHasAimr = false;
	Real fAimrWeight, fAimrBlend = 0.0f;
	Real fPropWeight [4] = {1,1,1,1};

	// Create temporary arrays
	hkArray<hkQsTransform> nextTransforms;
	nextTransforms.setSize( mSkelly->m_bones.getSize() );
	for ( int i = 0; i < nextTransforms.getSize(); ++i )
	{
		nextTransforms[i] = hkQsTransform(hkQsTransform::ZERO);
	}
	hkArray<hkQsTransform> tempTransforms;
	tempTransforms.setSize( mSkelly->m_bones.getSize() );
	tempTransforms = mSkelly->m_referencePose;

	hkArray<hkQsTransform> tempTransformsAdditive;
	tempTransformsAdditive.setSize( mSkelly->m_bones.getSize() );
	tempTransformsAdditive = mSkelly->m_referencePose;
	
	hkArray<hkQsTransform> aimrTransforms;
	aimrTransforms.setSize( mSkelly->m_bones.getSize() );
	aimrTransforms = mSkelly->m_referencePose;

	hkArray<hkQsTransform> refrTransforms;
	refrTransforms.setSize( mSkelly->m_bones.getSize() );
	refrTransforms = mSkelly->m_referencePose;

	hkArray<hkReal> tempReals;
	tempReals.setSize( mSkelly->m_bones.getSize() );

	//hkaAnimation* mhkAnim = ((CHKAnimationSet*)pAnimationSet)->GetHKAnimation();
	//hkaAnimation* mhkAnim = mMirrorAnim;
	/*if ( mhkAnim->m_numberOfTransformTracks == 0 )
	{
		std::cout << "Bad animation set on hkaAnimation: " << mhkAnim << std::endl;
		return;
	}*/
	std::vector<hkaAnimation*>* mAnim = ((CHKAnimationSet*)pAnimationSet)->GetHKAnimation();
	if ( !mMirrorAnims.empty() ) {
		mAnim = &mMirrorAnims;
	}

	// If no animations, just sample reference pose (frame0) and call it good
	if ( mAnimations.size() <= 2 )
	{
		//mhkAnim->sampleTracks( 1/30.0f, nextTransforms.begin(), tempReals.begin() );
		mAnim->at(0)->sampleTracks( 0.0f, nextTransforms.begin(), tempReals.begin() );

		hkaPose skeletonPose ( hkaPose::LOCAL_SPACE, mSkelly, nextTransforms );
		nextTransforms = skeletonPose.getSyncedPoseLocalSpace();

		for ( int i = 0; i < nextTransforms.getSize(); ++i )
		{
			((XTransform*)animRefs[i])->position = Vector3d(
				nextTransforms[i].m_translation.getComponent<0>(),
				nextTransforms[i].m_translation.getComponent<1>(),
				nextTransforms[i].m_translation.getComponent<2>() );
			((XTransform*)animRefs[i])->rotation = Quaternion(
				nextTransforms[i].m_rotation.m_vec.getComponent<0>(),
				nextTransforms[i].m_rotation.m_vec.getComponent<1>(),
				nextTransforms[i].m_rotation.m_vec.getComponent<2>(),
				nextTransforms[i].m_rotation.m_vec.getComponent<3>() );
			((XTransform*)animRefs[i])->scale = Vector3d(
				nextTransforms[i].m_scale.getComponent<0>(),
				nextTransforms[i].m_scale.getComponent<1>(),
				nextTransforms[i].m_scale.getComponent<2>() );
		}
		return;
	}

	{	// Set default aimrTransforms
		mAnim->at(0)->sampleTracks( 0.0f, aimrTransforms.begin(), tempReals.begin() );
		mAnim->at(0)->sampleTracks( 0.0f, refrTransforms.begin(), tempReals.begin() );
	}

	// Before sampling actions, need to check events
	if ( bEventsRead ) {
		vEvents.clear();
		bEventsRead = false;
	}
	// Reset model motion
	vModelMotion = Vector3d(0,0,0);

	// Need to update smooth faders
	for ( auto it = fadeOutList.begin(); it != fadeOutList.end(); )
	{
		it->first->weight -= deltaTime / it->second;
		if ( it->first->weight <= 0 ) {
			//it->first->isPlaying = false;
			//it->first->weight = 0;
			it->first->Stop();
			it = fadeOutList.erase(it);
		}
		else {
			++it;
		}
	}

	// Normalize weights
	for ( uchar i = 0; i < maxLayers; ++i ) {
		Normalize( i );
	}

	// First, loop through all the animations to setup sync tracks and the event system.
	CAnimAction*	syncMaxers [5] = {0,0,0,0,0};
	ftype			syncFrames [5] = {0,0,0,0,0};
	CAnimAction*	eventMaster = NULL;
	{
		std::map<string,CAnimAction>::iterator it = mAnimations.begin(); // Loop thru all the actions
		while ( it != mAnimations.end() )
		{
			CAnimAction& currentAction = it->second;
			
			// Setup sync-tracks
			if ( currentAction.sync_track )
			{
				if ( (syncMaxers[currentAction.sync_track] == NULL) || (syncMaxers[currentAction.sync_track]->weight < currentAction.weight) )
				{
					if ( syncMaxers[currentAction.sync_track] ) {
						syncMaxers[currentAction.sync_track]->SetMasterState( false );
					}
					currentAction.SetMasterState( true );
					syncMaxers[currentAction.sync_track] = &currentAction;
					syncFrames[currentAction.sync_track] = currentAction.frame;
				}
				else
				{
					currentAction.SetMasterState( false );
				}
			}

			// Setup event system
			currentAction.SetEventsState(false);
			//if ( eventMaster == NULL || (currentAction.weight > 0.04f && eventMaster->weight < currentAction.weight && eventMaster->layer >= currentAction.layer) )
			if ( (currentAction.weight > 0.04f && currentAction.isPlaying) && (eventMaster == NULL || (eventMaster->layer < currentAction.layer) || (eventMaster->weight < currentAction.weight && eventMaster->layer <= currentAction.layer)) )
			{
				if ( eventMaster != NULL ) {
					eventMaster->SetEventsState(false);
				}
				eventMaster = &currentAction;
				eventMaster->SetEventsState(true);
			}

			it++;
		}
	}

	// Loop through all the layers
	//if ( mAnimations.size() > 0 )
	//{
		for ( unsigned char layer = 0; layer < maxLayers; layer++ )
		{
			std::map<string,CAnimAction>::iterator it = mAnimations.begin(); // Loop thru all the actions
			do
			{
				CAnimAction& currentAction = it->second;

				// Check for a layer limit
				if ( currentAction.layer >= maxLayers )
					currentAction.layer = maxLayers-1;
				// And only sample+update the animation if it's on the current layer
				if ( currentAction.layer == layer )
				{
					// Update the animation
					currentAction.Update( deltaTime, syncFrames[currentAction.sync_track] );

					// Now, check the weight for valid values
					if ( currentAction.weight > 1 )
						currentAction.weight = 1;
					// And only sample if the weight is larger than zero
					if ( currentAction.weight > 0.001f )
					{
						ftype currentTime;
						//currentTime = ( currentAction.GetStart()+currentAction.frame ) * (1/30.0f);
						currentTime = currentAction.frame * (1/30.0f);

						// Increase the weight to full if this is the first animation to be sampled
						ftype temp = currentAction.weight;
						if ( bFirstAnim ) {
							currentAction.weight = 1;
						}
						// Increase the aimr weight to full and copy over the pose if this is the first aimr transform to be sampled
						if ( currentAction.tag == CAnimAction::TAG_ITEM ) {
							fAimrWeight = currentAction.weight;
							if ( !bHasAimr ) {
								// Set weight to full
								fAimrWeight = 1;
								bHasAimr = true;
							}
						}

						// Sample tracks
						//mhkAnim->sampleTracks( currentTime, tempTransforms.begin(), tempReals.begin() );
						mAnim->at(currentAction.index)->sampleTracks( currentTime, tempTransforms.begin(), tempReals.begin() );
						// Sample track first frame if needed
						if ( currentAction.extrapolateMotion[0] || currentAction.extrapolateMotion[1] || currentAction.extrapolateMotion[2] ) {
							//mAnim->at(0)->sampleTracks( currentAction.GetStart() * (1/30.0f), tempTransformsAdditive.begin(), tempReals.begin() );
							mAnim->at(currentAction.index)->sampleTracks( 0.0f, tempTransformsAdditive.begin(), tempReals.begin() );
						}

						// Mix the tracks
						std::vector<int> *pMixingList = currentAction.GetMixingList();
						if ( pMixingList->size() == 0 )
						{
							// Mix into the entire list
							for ( int i = 0; i < tempTransforms.getSize(); ++i )
							{
								// Aimer IK
								if ( currentAction.tag == CAnimAction::TAG_ITEM ) {
									aimrTransforms[i].setInterpolate4( aimrTransforms[i], tempTransforms[i], fAimrWeight );
									fAimrBlend += fAimrWeight;
								}
								// Motion extrapolation
								if ( i == 0 ) {
									Real speed = (currentAction.framesPerSecond/30.0f) * currentAction.playSpeed * (deltaTime * 30.0f);
									Real t_weight = currentAction.weight * speed;
									if ( currentAction.extrapolateMotion[0] ) {
										if ( currentAction.enableMotionExtrapolation[0] ) {
											vModelMotion.x += t_weight * (tempTransforms[i].getTranslation().getComponent<0>() - tempTransformsAdditive[i].getTranslation().getComponent<0>());
										}
										tempTransforms[i].m_translation.setComponent<0>( tempTransformsAdditive[i].getTranslation().getComponent<0>() );
									}
									if ( currentAction.extrapolateMotion[1] ) {
										if ( currentAction.enableMotionExtrapolation[1] ) {
											vModelMotion.y += t_weight * (tempTransforms[i].getTranslation().getComponent<1>() - tempTransformsAdditive[i].getTranslation().getComponent<1>());
										}
										tempTransforms[i].m_translation.setComponent<1>( tempTransformsAdditive[i].getTranslation().getComponent<1>() );
									}
									if ( currentAction.extrapolateMotion[2] ) {
										if ( currentAction.enableMotionExtrapolation[2] ) {
											vModelMotion.z += t_weight * (tempTransforms[i].getTranslation().getComponent<2>() - tempTransformsAdditive[i].getTranslation().getComponent<2>());
										}
										tempTransforms[i].m_translation.setComponent<2>( tempTransformsAdditive[i].getTranslation().getComponent<2>() );
									}
								}
								// Last, sample the animation
								nextTransforms[i].setInterpolate4( nextTransforms[i], tempTransforms[i], currentAction.weight );
							}
						}
						else
						{
							// Mix into the list specially
							int index;
							for ( uint32_t i = 0; i < pMixingList->size(); ++i )
							{
								index = (*pMixingList)[i];
								nextTransforms[index].setInterpolate4( nextTransforms[index], tempTransforms[index], currentAction.weight );
								// Aimer IK
								if ( currentAction.tag == CAnimAction::TAG_ITEM ) {
									aimrTransforms[index].setInterpolate4( aimrTransforms[index], tempTransforms[index], fAimrWeight );
								}
								// Motion extrapolation
								if ( index == 0 ) {
									if ( currentAction.extrapolateMotion[0] && currentAction.enableMotionExtrapolation[0] ) {
										vModelMotion.x += currentAction.weight * tempTransforms[index].getTranslation().getComponent<0>();
									}
									if ( currentAction.extrapolateMotion[1] && currentAction.enableMotionExtrapolation[1] ) {
										vModelMotion.y += currentAction.weight * tempTransforms[index].getTranslation().getComponent<1>();
									}
									if ( currentAction.extrapolateMotion[2] && currentAction.enableMotionExtrapolation[2] ) {
										vModelMotion.z += currentAction.weight * tempTransforms[index].getTranslation().getComponent<2>();
									}
								}
							}
						}

						if ( currentAction.tag == CAnimAction::TAG_ITEM && bFirstAimr ) {
							// Mix into the entire list
							for ( int i = 0; i < tempTransforms.getSize(); ++i )
							{
								//nextTransforms[i].setInterpolate4( nextTransforms[i], tempTransforms[i], currentAction.weight );
								// Aimer IK
								aimrTransforms[i].setInterpolate4( aimrTransforms[i], tempTransforms[i], fAimrWeight );
								fAimrBlend += fAimrWeight;
							}
						}

						// Perform prop weights
						fPropWeight[0] -= currentAction.prop_override[0];
						fPropWeight[1] -= currentAction.prop_override[1];
						fPropWeight[2] -= currentAction.prop_override[2];
						fPropWeight[3] -= currentAction.prop_override[3];

						// Restore previous weight
						currentAction.weight = temp;
						bFirstAnim = false;	// No longer first animation to be sampled.
						if ( bHasAimr ) {
							bFirstAimr = false;
						}
					}
				}

				it++;
			}
			while ( it != mAnimations.end() );
		}
	//}

	// If no animations sampled, grab first frame
	if ( bFirstAnim )
	{
		//mhkAnim->sampleTracks( 1/30.0f, tempTransforms.begin(), tempReals.begin() );
		mAnim->at(0)->sampleTracks( 0.0f, tempTransforms.begin(), tempReals.begin() );
		// Mix into the entire list
		for ( int i = 0; i < tempTransforms.getSize(); ++i )
		{
			nextTransforms[i] = tempTransforms[i];
		}
	}

	/*if ( bHasAimr )
	{
		// Mix into the entire list
		for ( int i = 0; i < tempTransforms.getSize(); ++i )
		{
			nextTransforms[i] = aimrTransforms[i];
		}
	}*/

#if defined(_ENGINE_DEBUG) && defined(_ENGINE_SAFE_CHECK_)
	// CHECK FUCKING EVERYTHING
	for ( int i = 0; i < tempTransforms.getSize(); ++i ) {
		if ( !tempTransforms[i].isOk() )
			cout << "BAD SHIT ON TT" << i << endl;
	}
	for ( int i = 0; i < nextTransforms.getSize(); ++i ) {
		if ( !nextTransforms[i].isOk() )
			cout << "BAD SHIT ON NT" << i << endl;
	}
	for ( int i = 0; i < aimrTransforms.getSize(); ++i ) {
		if ( !aimrTransforms[i].isOk() )
			cout << "BAD SHIT ON AM" << i << endl;
	}
#endif
	fAimrBlend = std::max<ftype>( 0, std::min<ftype>( 1, fAimrBlend ) );

	// Create the pose
	hkaPose skeletonPose ( hkaPose::LOCAL_SPACE, mSkelly, nextTransforms );
	hkaPose referencePose ( hkaPose::LOCAL_SPACE, mSkelly, mSkelly->m_referencePose );
	//skeletonPose.syncModelSpace();
	//referencePose.syncModelSpace();

	// Grab the proper model scale
	hkVector4 t_modelScale;
	{
		t_modelScale = mSkelly->m_referencePose[0].m_scale;
		if ( fabs( t_modelScale.getComponent<0>().getReal() - 1.0f ) < 0.01f )
		{
			t_modelScale = mSkelly->m_referencePose[1].m_scale;
		}
	}

	// Create the model-to-world and world-to-model transforms
	hkQsTransform modelToWorld;
	{
		// First scale up by mSkelly->m_referencePose[0].m_scale
		// Next translate by skeletonPose.getBoneModelSpace( 0 ).getTranslation()
		// Next rotate by model->transform.rotation
		// Next translate by model->transform.position
		hkVector4 trans1pos = skeletonPose.getBoneModelSpace( 0 ).getTranslation();
		//trans1pos.div( mSkelly->m_referencePose[0].m_scale );
		hkQsTransform trans0(hkQsTransform::IDENTITY); trans0.setTranslation( trans1pos );
		//hkQuaternion trans1quat; trans1quat.setInverse( skeletonPose.getBoneModelSpace( 0 ).getRotation() );
		hkQsTransform trans1(hkQsTransform::IDENTITY); //trans1.setRotation( trans1quat );
		hkQsTransform trans2(hkQsTransform::IDENTITY); trans2.setScale( t_modelScale );
		//Quaternion trans3quat = pOwner->transform.rotation.getQuaternion();
		Quaternion trans3quat = mModelTransform.rotation;
		hkQsTransform trans3(hkQsTransform::IDENTITY); trans3.setRotation( hkQuaternion(trans3quat.x,trans3quat.y,trans3quat.z,trans3quat.w) );
		//hkQsTransform trans4(hkQsTransform::IDENTITY); trans4.setTranslation( hkVector4( pOwner->transform.position.x, pOwner->transform.position.y, pOwner->transform.position.z ) );
		hkQsTransform trans4(hkQsTransform::IDENTITY); trans4.setTranslation( hkVector4( mModelTransform.position.x, mModelTransform.position.y, mModelTransform.position.z ) );

		hkQsTransform transt0, transt1;
				transt0.setMulScaled( trans2, trans1 );
				transt1.setMulScaled( trans0, transt0 );
				transt0.setMulScaled( trans3, transt1 );
		modelToWorld.setMulScaled( trans4, transt0 );
	}
	hkQsTransform worldToModel;
	{
		// First move to engine-model space by subtracting position, then rotating by model inverse
		// Then subtract by the root bone
		// Then scale down by the scaling
		//hkQsTransform trans0(hkQsTransform::IDENTITY); trans0.setTranslation( hkVector4( -pOwner->transform.position.x, -pOwner->transform.position.y, -pOwner->transform.position.z ) );
		hkQsTransform trans0(hkQsTransform::IDENTITY); trans0.setTranslation( hkVector4( -mModelTransform.position.x, -mModelTransform.position.y, -mModelTransform.position.z ) );
		//Quaternion trans1quat = pOwner->transform.rotation.inverse().getQuaternion();
		Quaternion trans1quat = Rotator(mModelTransform.rotation).inverse().getQuaternion();
		hkQsTransform trans1(hkQsTransform::IDENTITY); trans1.setRotation( hkQuaternion(trans1quat.x,trans1quat.y,trans1quat.z,trans1quat.w) );
		hkVector4 trans2pos; trans2pos.setNeg3( skeletonPose.getBoneModelSpace( 0 ).getTranslation() ); trans2pos.zeroElement(3);
		hkQsTransform trans2(hkQsTransform::IDENTITY); trans2.setTranslation( trans2pos );
		hkVector4 trans3scal (1,1,1,1); trans3scal.div4( t_modelScale ); trans3scal.zeroElement(3);
		hkQsTransform trans3(hkQsTransform::IDENTITY); trans3.setScale( trans3scal );

		hkQsTransform transt0, transt1;
				transt0.setMulScaled( trans1, trans0 );
				transt1.setMulScaled( trans2, transt0 );
		worldToModel.setMulScaled( trans3, transt1 );
	}

	// Do IK
	int32_t headBone = -1, leyeBone = -1, reyeBone = -1;
	int32_t	t_footIK_count = 0;
	int32_t t_aimrIK_count = 0;
	hkReal	t_verticalOffset = 0;
	Real	t_aimrIK_pushval = 0;
	// Do IK Prepass
	for ( uint i = 0; i < ikList.size(); ++i )
	{
		switch ( ikList[i].type )
		{
		case IK_LOOKAT:
			headBone = ikList[i].bone[0];
			leyeBone = ikList[i].bone[1];
			reyeBone = ikList[i].bone[2];
			break;
		case IK_FOOTSTEP:
			{
				// Perform ankle changes (lift up the character, because of digitgrade feet)
				if ( ikList[i].subinfo[0] > FTYPE_PRECISION )
				{
					// Calculate amount the model needs to be pushed up by
					hkReal verticalValue = 0;
					{
						hkVector4 footDistance = skeletonPose.getBoneModelSpace( ikList[i].bone[0] ).m_translation;
						footDistance.setComponent<1>( footDistance.getComponent<1>().getReal() * 0.1f );
						footDistance.setComponent<2>(0);

						verticalValue += std::max<hkReal>( 0, 20.0f - footDistance.length3() ) * 0.015f;
					}
					// Push up the model
					hkVector4 verticalOffset ( 0,0,verticalValue * ikList[i].subinfo[0] );
					skeletonPose.accessBoneModelSpace( 0, hkaPose::PROPAGATE ).m_translation.add( verticalOffset );
					t_verticalOffset += verticalValue;
				}
			}
			break;
		}
	}
	// Do IK Main Pass
	for ( uint i = 0; i < ikList.size(); ++i )
	{
		switch ( ikList[i].type )
		{
		case IK_LOOKAT:
			if ( ikList[i].enabled )
			{
				// Apply first transform based on old model rotation
				/*hkQuaternion prev_feedback( ikList[i].subinfo[0], ikList[i].subinfo[1], ikList[i].subinfo[2], ikList[i].subinfo[3] );
				if ( prev_feedback.isOk() )
				{
					skeletonPose.accessBoneLocalSpace( headBone ).m_rotation.setSlerp( skeletonPose.accessBoneLocalSpace( headBone ).m_rotation, prev_feedback, 0.5 );
					skeletonPose.syncModelSpace();
				}*/

				// Do the Head IK

				// Generate the Eye offset from the head base
				int32_t leyeBone = ikList[i].bone[1];
				int32_t reyeBone = ikList[i].bone[2];
				hkVector4 offset = referencePose.getBoneLocalSpace(leyeBone).getTranslation();
				offset.add( referencePose.getBoneLocalSpace(reyeBone).getTranslation() );
				offset.mul( 0.5f );
				offset.set( offset.getComponent(2), -offset.getComponent(1), offset.getComponent(0) );

				hkVector4 temp;
				// Create the target
				hkVector4 target = hkVector4( ikList[i].input.x, ikList[i].input.y, ikList[i].input.z );
				temp.setTransformedPos( worldToModel, target );
				//temp.sub( hkVector4(0,0,t_verticalOffset) );
				target = temp;

				// Create the second target
				hkVector4 subtarget = hkVector4( ikList[i].subinput0.x, ikList[i].subinput0.y, ikList[i].subinput0.z );
				temp.setTransformedPos( worldToModel, subtarget );
				//temp.sub( hkVector4(0,0,t_verticalOffset) );
				subtarget = temp;

				// "pose" is an hkaPose object that contains the current pose of our character
				hkaLookAtIkSolver::Setup setup;
				setup.m_fwdLS.set( 0,1,0 );
				setup.m_eyePositionLS = offset;
				setup.m_limitAxisMS.set( 0,-1,0 );
				setup.m_limitAxisMS.setRotatedDir( skeletonPose.getBoneModelSpace(ikList[i].bone[0]-1).getRotation(), hkVector4(0,1,0) );
				setup.m_limitAngle = HK_REAL_PI / 2.6f;
				// By using the PROPAGATE flag, all children of the head bone will be modified alongside the head
				// (their local transforms will remain constant)
				hkQuaternion t_prevrot = skeletonPose.getBoneModelSpace( headBone ).m_rotation;
				hkaLookAtIkSolver::solve (
					setup, target,
					1.0f, skeletonPose.accessBoneModelSpace( headBone,hkaPose::PROPAGATE )
					);
				//skeletonPose.syncLocalSpace();

				// Solve for the eyes now
				subtarget.setComponent<2>( subtarget.getComponent<2>() + offset.getComponent<0>() );
				setup.m_fwdLS.set( 1,0,0 );
				setup.m_eyePositionLS.set( 0,0,0 );
				setup.m_limitAxisMS.setRotatedDir( skeletonPose.getBoneModelSpace(ikList[i].bone[0]).getRotation(), hkVector4(0,1,0) );
				setup.m_limitAngle = HK_REAL_PI / 1.0f;
				hkaLookAtIkSolver::solve (
					setup, subtarget,
					1.0f, skeletonPose.accessBoneModelSpace( leyeBone,hkaPose::PROPAGATE )
					);
				setup.m_limitAxisMS.setRotatedDir( skeletonPose.getBoneModelSpace(ikList[i].bone[0]).getRotation(), hkVector4(0,1,0) );
				hkaLookAtIkSolver::solve (
					setup, subtarget,
					1.0f, skeletonPose.accessBoneModelSpace( reyeBone,hkaPose::PROPAGATE )
					);
				//skeletonPose.syncLocalSpace();

				// Now feedback the head rotation
				hkQuaternion feedback = skeletonPose.getBoneLocalSpace( headBone ).getRotation();
				ikList[i].subinfo[0] = feedback.getComponent<0>();
				ikList[i].subinfo[1] = feedback.getComponent<1>();
				ikList[i].subinfo[2] = feedback.getComponent<2>();
				ikList[i].subinfo[3] = feedback.getComponent<3>();
			}
			break;
		case IK_AIMING:
			if ( true /*bHasAimr && ikList[i].enabled*/ )
			{
				// Do AIMING IK
				hkaPose aimrPose ( hkaPose::LOCAL_SPACE, mSkelly, aimrTransforms );
				hkaPose refrPose ( hkaPose::LOCAL_SPACE, mSkelly, refrTransforms );

				//skeletonPose.syncModelSpace(); // Update the model space first

				t_aimrIK_count += 1;
				// Only perform spine aiming on the first aimer
				if ( t_aimrIK_count == 1 )
				{
					hkQsTransform	spineTransform;
					hkQuaternion	offsetRotation;
					hkQuaternion	resultRotation;

					animLerper		lerpHelp(deltaTime);

					// Perform spine rotations
					Real t_weightSpine = ikList[i].subinput0.y;
					if ( t_weightSpine > 0 )
					{
						lerpHelp( mSpineBlends[0], t_weightSpine );

						spineTransform = skeletonPose.getBoneModelSpace( ikList[i].bone[0] );
						offsetRotation.setAxisAngle( hkVector4(1,0,0), (Real)-degtorad(ikList[i].input.z*mSpineBlends[0]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						spineTransform.m_rotation = resultRotation;
						offsetRotation.setAxisAngle( hkVector4(0,0,1), (Real)-degtorad(ikList[i].input.x*mSpineBlends[0]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						
						spineTransform.m_rotation = resultRotation;
						skeletonPose.setBoneModelSpace( ikList[i].bone[0], spineTransform, hkaPose::PROPAGATE );

						t_aimrIK_pushval += mSpineBlends[0];
					}
					t_weightSpine -= ikList[i].subinput0.z;
					if ( t_weightSpine > 0 )
					{
						lerpHelp( mSpineBlends[1], t_weightSpine );

						spineTransform = skeletonPose.getBoneModelSpace( ikList[i].bone[1] );
						offsetRotation.setAxisAngle( hkVector4(1,0,0), (Real)-degtorad(ikList[i].input.z*mSpineBlends[1]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						spineTransform.m_rotation = resultRotation;
						offsetRotation.setAxisAngle( hkVector4(0,0,1), (Real)-degtorad(ikList[i].input.x*mSpineBlends[1]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						
						spineTransform.m_rotation = resultRotation;
						skeletonPose.setBoneModelSpace( ikList[i].bone[1], spineTransform, hkaPose::PROPAGATE );

						t_aimrIK_pushval += mSpineBlends[1];
					}
					t_weightSpine -= ikList[i].subinput0.z;
					if ( t_weightSpine > 0 )
					{
						lerpHelp( mSpineBlends[2], t_weightSpine );

						spineTransform = skeletonPose.getBoneModelSpace( ikList[i].bone[2] );
						offsetRotation.setAxisAngle( hkVector4(1,0,0), (Real)-degtorad(ikList[i].input.z*mSpineBlends[2]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						spineTransform.m_rotation = resultRotation;
						offsetRotation.setAxisAngle( hkVector4(0,0,1), (Real)-degtorad(ikList[i].input.x*mSpineBlends[2]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						
						spineTransform.m_rotation = resultRotation;
						skeletonPose.setBoneModelSpace( ikList[i].bone[2], spineTransform, hkaPose::PROPAGATE );
					
						t_aimrIK_pushval += mSpineBlends[2];
					}
					// Perform additional faceat rotations
					{
						spineTransform = skeletonPose.getBoneModelSpace( ikList[i].bone[0] );
						offsetRotation.setAxisAngle( hkVector4(0,0,1), (Real)-degtorad(ikList[i].subinput1.y*0.444f) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						
						spineTransform.m_rotation = resultRotation;
						skeletonPose.setBoneModelSpace( ikList[i].bone[0], spineTransform, hkaPose::PROPAGATE );
					}
					{
						spineTransform = skeletonPose.getBoneModelSpace( ikList[i].bone[1] );
						offsetRotation.setAxisAngle( hkVector4(0,0,1), (Real)-degtorad(ikList[i].subinput1.y*0.333f) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						
						spineTransform.m_rotation = resultRotation;
						skeletonPose.setBoneModelSpace( ikList[i].bone[1], spineTransform, hkaPose::PROPAGATE );
					}
					{
						spineTransform = skeletonPose.getBoneModelSpace( ikList[i].bone[2] );
						offsetRotation.setAxisAngle( hkVector4(0,0,1), (Real)-degtorad(ikList[i].subinput1.y*0.222f) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						
						spineTransform.m_rotation = resultRotation;
						skeletonPose.setBoneModelSpace( ikList[i].bone[2], spineTransform, hkaPose::PROPAGATE );
					}
					// Perform neck and head rotations
					{
						lerpHelp( mSpineBlends[3], ikList[i].subinput0.x );

						spineTransform = skeletonPose.getBoneModelSpace( headBone );
						offsetRotation.setAxisAngle( hkVector4(1,0,0), (Real)-degtorad(ikList[i].input.z*mSpineBlends[3]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						spineTransform.m_rotation = resultRotation;
						offsetRotation.setAxisAngle( hkVector4(0,0,1), (Real)-degtorad(ikList[i].input.x*mSpineBlends[3]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );

						spineTransform.m_rotation = resultRotation;
						skeletonPose.setBoneModelSpace( headBone, spineTransform, hkaPose::PROPAGATE );
					
						t_aimrIK_pushval += mSpineBlends[3];
					}
					{
						lerpHelp( mSpineBlends[4], ikList[i].subinput1.x );

						spineTransform = skeletonPose.getBoneModelSpace( headBone-1 );
						offsetRotation.setAxisAngle( hkVector4(1,0,0), (Real)-degtorad(ikList[i].input.z*mSpineBlends[4]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );
						spineTransform.m_rotation = resultRotation;
						offsetRotation.setAxisAngle( hkVector4(0,0,1), (Real)-degtorad(ikList[i].input.x*mSpineBlends[4]) );
						resultRotation.setMul( offsetRotation, spineTransform.m_rotation );

						spineTransform.m_rotation = resultRotation;
						skeletonPose.setBoneModelSpace( headBone-1, spineTransform, hkaPose::PROPAGATE );
					
						t_aimrIK_pushval += mSpineBlends[4];
					}

					// Update all space
					//skeletonPose.syncAll();
				}

			if ( bHasAimr )
			{
				for ( uint bb = 3; bb < 5; ++bb )
				{
					// First get the target position in model space
					hkQsTransform wristModelSource		= aimrPose.getBoneModelSpace(ikList[i].bone[bb]+2); // Wrist bone

					hkQsTransform headModelAimer		= aimrPose.getBoneModelSpace(headBone);
					hkQsTransform leyeModelAimer		= aimrPose.getBoneModelSpace(leyeBone);
					hkQsTransform reyeModelAimer		= aimrPose.getBoneModelSpace(reyeBone);
					hkQsTransform headModelTrans		= skeletonPose.getBoneModelSpace(headBone);
					hkQsTransform leyeModelTrans		= skeletonPose.getBoneModelSpace(leyeBone);
					hkQsTransform reyeModelTrans		= skeletonPose.getBoneModelSpace(reyeBone);
					hkVector4 vBasePos, vOffsetPos;
					//  vBasePos.setAdd( leyeModelAimer.m_translation, reyeModelAimer.m_translation );
					//vOffsetPos.setAdd( leyeModelTrans.m_translation, reyeModelTrans.m_translation );
					  vBasePos = headModelAimer.m_translation;
					vOffsetPos = headModelTrans.m_translation;

					hkVector4 vHeadDistance;
					vHeadDistance.setSub( wristModelSource.getTranslation(), vBasePos );

					// Rotate the dif vect
					hkQuaternion lookatRot;
					Real t_armOffset = std::max<ftype>( ikList[i].input.z, -10-70*ikList[i].subinfo[2] ) * std::max<ftype>( 0, 1.0f - t_aimrIK_pushval + 0.2f*ikList[i].subinfo[2] );
					lookatRot.setAxisAngle( hkVector4(1,0,0), -(hkReal)degtorad(t_armOffset) );
					vHeadDistance.setRotatedDir( lookatRot, vHeadDistance );

					// Move to current head space
					hkVector4 vInvRotatedHeadDistance;
					vInvRotatedHeadDistance.setRotatedInverseDir( headModelAimer.m_rotation, vHeadDistance );
					vHeadDistance.setRotatedDir( headModelTrans.m_rotation, vInvRotatedHeadDistance );

					// Generate the target position by using the current skeleton
					hkVector4 targetPosition;
					targetPosition.setAdd( vHeadDistance, vOffsetPos );
/*						
#ifdef _ENGINE_DEBUG	// Draw the wrist positions
					hkVector4 targetWorldPosition;
					targetWorldPosition.setTransformedPos( modelToWorld, targetPosition );
					Vector3d result ( targetWorldPosition.getComponent<0>(), targetWorldPosition.getComponent<1>(), targetWorldPosition.getComponent<2>() );
					Debug::Drawer->DrawLine( result-Vector3d(0,0.5f,0), result+Vector3d(0,0.5f,0), Color(1,0,1,1) );
					Debug::Drawer->DrawLine( result-Vector3d(0,0,0.5f), result+Vector3d(0,0,0.5f), Color(1,0,1,1) );
					Debug::Drawer->DrawLine( result-Vector3d(0.5f,0,0), result+Vector3d(0.5f,0,0), Color(1,0,1,1) );
#endif
*/
					// Generate the target rotation
					hkQuaternion targetRotation = wristModelSource.getRotation();
					//lookatRot.setAxisAngle( hkVector4(1,0,0), -(hkReal)degtorad(std::min<ftype>( (ikList[i].input.z*0.5f) - 25.0f, 5 )) );
					lookatRot.setAxisAngle( hkVector4(1,0,0), -(hkReal)degtorad(ikList[i].input.z * 0.7f) );
					//lookatRot.setAxisAngle( hkVector4(1,0,0), -(hkReal)degtorad(t_armOffset) );
					targetRotation.setMul( lookatRot, wristModelSource.getRotation() );

					// Setup an IK to run on the arm
					hkaTwoJointsIkSolver::Setup ik_info;
					ik_info.m_firstJointIdx		= ikList[i].bone[bb];
					ik_info.m_secondJointIdx	= ikList[i].bone[bb]+1;
					ik_info.m_endBoneIdx		= ikList[i].bone[bb]+2;
						
					if ( bb == 4 ) { // Right arm
						ik_info.m_hingeAxisLS.set( 0,0,1 );
						ik_info.m_enforceEndRotation = true;
						ik_info.m_enforceEndPosition = true;
					}
					else if ( bb == 3 ) { // Left arm
						ik_info.m_hingeAxisLS.set( 0,0,1 );
						ik_info.m_enforceEndRotation = true;
						ik_info.m_enforceEndPosition = true;
					}
					ik_info.m_endTargetMS			= targetPosition;
					ik_info.m_endTargetRotationMS	= targetRotation;

					// Set IK Strength
					ik_info.m_firstJointIkGain	= fAimrBlend * ikList[i].subinfo[3] * ikList[i].subinfo[bb-3];
					ik_info.m_secondJointIkGain	= fAimrBlend * ikList[i].subinfo[3] * ikList[i].subinfo[bb-3];
					ik_info.m_endJointIkGain	= fAimrBlend * ikList[i].subinfo[3] * ikList[i].subinfo[bb-3];

					// Now, run an IK on the final pose
					hkaTwoJointsIkSolver::solve( ik_info, skeletonPose );
				}

				// Get result
				//skeletonPose.syncAll();
				//skeletonPose.syncModelSpace();
			}
		}
			break;
		case IK_FOOTSTEP:
			if ( ikList[i].enabled )
			{
				// subinfo0 is the amount of offset ankles
				// subinfo3 is the calculted weight in
				// subinfo2 is the saved foot fraction
				// subinfo1 is the foot fader. If above 0.5, then it enables the foot IK. If it is below 0.5 it disables the foot IK similar to how when the target leaves the ground.

				// Perform ankle changes
				if ( ikList[i].subinfo[0] > FTYPE_PRECISION )
				{
					hkQsTransform ankleTransform = skeletonPose.accessBoneLocalSpace( ikList[i].bone[0] );
					hkQuaternion targetAngle ( hkVector4(0,0,1), -HK_REAL_PI/6 );
					ankleTransform.m_rotation.setSlerp( hkQuaternion(ankleTransform.m_rotation), targetAngle, ikList[i].subinfo[0] );
					//skeletonPose.setBoneLocalSpace( ikList[i].bone[0], ankleTransform );
					skeletonPose.accessBoneLocalSpace( ikList[i].bone[0] ).setRotation( ankleTransform.m_rotation );
					//skeletonPose.syncLocalSpace();
					//skeletonPose.syncModelSpace();
					//skeletonPose.syncAll();
				}

				// Setup the Joint IK
				hkaTwoJointsIkSolver::Setup ik_setup;
				ik_setup.m_firstJointIdx	= ikList[i].bone[2];
				ik_setup.m_secondJointIdx	= ikList[i].bone[1];
				ik_setup.m_endBoneIdx		= ikList[i].bone[4];

				ik_setup.m_hingeAxisLS		= hkVector4( 0,0,1 );

				ik_setup.m_firstJointIkGain		= ikList[i].subinfo[3];
				ik_setup.m_secondJointIkGain	= ikList[i].subinfo[3];

				ik_setup.m_enforceEndPosition	= true;
				ik_setup.m_enforceEndRotation	= false; // Do not change end rotation

				// Take the toe position, and raycast it.
				{
					hkVector4 toeModelSpace = skeletonPose.accessBoneModelSpace( ikList[i].bone[3] ).getTranslation();
					hkVector4 toeWorldSpace;
					toeWorldSpace.setTransformedPos( modelToWorld, toeModelSpace );
					hkVector4 fromPosition = toeWorldSpace;
					fromPosition.add( hkVector4( 0,0,2.0f ) );
					hkVector4 toPosition = toeWorldSpace;
					toPosition.add( hkVector4( 0,0,-2.0f ) );

					// Cast downwards
					hkReal		resultFraction;
					hkVector4	resultNormal;
					hkWorldCaster lclCaster;
					hkBool hitGround = lclCaster.castRay( fromPosition, toPosition, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ), resultFraction, resultNormal );

					if ( hitGround && ikList[i].subinfo[1] > 0.5f )
					{
						// Fade in IK
						ikList[i].subinfo[3] = std::min<Real>( 1, ikList[i].subinfo[3]+deltaTime*2.0f );
						// Save the result fraction
						ikList[i].subinfo[2] = resultFraction;
					}
					else
					{
						// Fade out IK
						ikList[i].subinfo[3] = std::max<Real>( 0, ikList[i].subinfo[3]-deltaTime*3.0f );
						// Set fraction to the saved value
						resultFraction = ikList[i].subinfo[2];//0.67f;
					}
					// Perform the IK regardless
					if ( ikList[i].subinfo[3] > FTYPE_PRECISION )
					{
						hkVector4 groundPosition;
						groundPosition.setInterpolate( fromPosition, toPosition, resultFraction );
/*
#ifdef _ENGINE_DEBUG	// Draw the toe positions
						Vector3d result ( toeWorldSpace.getComponent<0>(), toeWorldSpace.getComponent<1>(), toeWorldSpace.getComponent<2>() );
						Debug::Drawer->DrawLine( result-Vector3d(0,0.5f,0), result+Vector3d(0,0.5f,0), Color(1,0,1,1) );
						Debug::Drawer->DrawLine( result-Vector3d(0,0,0.5f), result+Vector3d(0,0,0.5f), Color(1,0,1,1) );
						Debug::Drawer->DrawLine( result-Vector3d(0.5f,0,0), result+Vector3d(0.5f,0,0), Color(1,0,1,1) );
#endif*/
						// Z is the ground. Compare with parent model Z=0
						hkVector4 targetPosition = toeWorldSpace;
						//hkReal zOffset = groundPosition.getComponent<2>().getReal() - pOwner->transform.position.z + ikList[i].subinfo[0]*(-t_verticalOffset+0.57f);
						hkReal zOffset = groundPosition.getComponent<2>().getReal() - mModelTransform.position.z + ikList[i].subinfo[0]*(-t_verticalOffset+0.57f);
						targetPosition.setComponent<2>( targetPosition.getComponent<2>().getReal() + zOffset );
						// Move the target position to model space
						ik_setup.m_endTargetMS.setTransformedPos( worldToModel, targetPosition );
						// Perform the IK
						hkaTwoJointsIkSolver::solve( ik_setup, skeletonPose );
					}
				}
			}
			t_footIK_count += 1;
			break;
		case IK_PROPS:
			//if ( ikList[i].enabled )
			{
				animLerper		lerpHelp(deltaTime);

				fPropWeight[0] = std::max<Real>( 0, fPropWeight[0] );
				fPropWeight[1] = std::max<Real>( 0, fPropWeight[1] );
				fPropWeight[2] = std::max<Real>( 0, fPropWeight[2] );
				fPropWeight[3] = std::max<Real>( 0, fPropWeight[3] );

				lerpHelp( mPropBlends[0], fPropWeight[0] );
				lerpHelp( mPropBlends[1], fPropWeight[1] );
				lerpHelp( mPropBlends[2], fPropWeight[2] );
				lerpHelp( mPropBlends[3], fPropWeight[3] );

				// Loop through the prop bones. If the animations have nothing to say about it, give it a default prop value 
				for ( uint bb = 0; bb < 4; ++bb )
				{
					int32_t propIndex = ikList[i].bone[bb];

					if ( propIndex && (mPropBlends[bb] > FTYPE_PRECISION) )
					{
						hkQuaternion targetRotation;
						targetRotation.setMul( hkQuaternion( hkVector4(0,1,0), HK_REAL_PI/2 ), hkQuaternion( hkVector4(1,0,0), HK_REAL_PI/2 ) );
						hkVector4 targetPosition;
						targetPosition = hkVector4( 1.4f,0.8f,-1.0f );

						hkQuaternion sourceRotation = skeletonPose.accessBoneLocalSpace(propIndex).m_rotation;
						hkVector4	 sourcePosition = skeletonPose.accessBoneLocalSpace(propIndex).m_translation;
						skeletonPose.accessBoneLocalSpace(propIndex).m_rotation.setSlerp( sourceRotation, targetRotation, mPropBlends[bb] );
						skeletonPose.accessBoneLocalSpace(propIndex).m_translation.setInterpolate( sourcePosition, targetPosition, mPropBlends[bb] );
					}
				}
			}
			break;
		}
	}

	// Rotate all bones 180 degrees around X+
	/*for ( uint i = 0; i < nextTransforms.getSize(); ++i )
	{
		hkQsTransform t_transform = skeletonPose.getBoneModelSpace(i);

		//t_transform.m_rotation.mul( hkQuaternion(hkVector4(1,0,0),PI) );
		t_transform.m_rotation.setMul( t_transform.m_rotation, hkQuaternion(hkVector4(1,0,0),PI) );

		//hkVector4 t_axis;
		//t_axis.setRotatedDir( t_transform.m_rotation, hkVector4(1,0,0) );
		//t_transform.m_rotation.mul( hkQuaternion(t_axis, PI) );

		skeletonPose.setBoneModelSpace( i, t_transform, hkaPose::DONT_PROPAGATE );
	}*/

	/*
	// Animation flip
	for ( uint i = 0; i < nextTransforms.getSize(); ++i )
	{
		hkQsTransform t_transform = skeletonPose.getBoneModelSpace(i);

		//t_transform.m_rotation.mul( hkQuaternion(hkVector4(1,0,0),PI) );
		//t_transform.m_rotation.setMul( t_transform.m_rotation, hkQuaternion(hkVector4(1,0,0),PI) );
		t_transform.m_translation.set(
			-t_transform.m_translation.getComponent<0>(),
			t_transform.m_translation.getComponent<1>(),
			t_transform.m_translation.getComponent<2>(),
			t_transform.m_translation.getComponent<3>()
			);
		hkRotation t_rot;
		t_rot.set( t_transform.m_rotation );
		hkVector4 t_row;

		t_row = t_rot.getColumn<1>();
		t_row.setNeg<3>( t_row );
		t_rot.setColumn<1>( t_row );

		t_row = t_rot.getColumn<2>();
		t_row.setNeg<3>( t_row );
		t_rot.setColumn<2>( t_row );
		
		t_transform.m_rotation.set( t_rot );


		t_transform.m_scale.set(
			t_transform.m_scale.getComponent<0>(),
			t_transform.m_scale.getComponent<1>(),
			t_transform.m_scale.getComponent<2>(),
			t_transform.m_scale.getComponent<3>()
			);

		//hkVector4 t_axis;
		//t_axis.setRotatedDir( t_transform.m_rotation, hkVector4(1,0,0) );
		//t_transform.m_rotation.mul( hkQuaternion(t_axis, PI) );

		skeletonPose.setBoneModelSpace( i, t_transform, hkaPose::DONT_PROPAGATE );
	}*/

	nextTransforms = skeletonPose.getSyncedPoseLocalSpace();
	//tempTransforms = skeletonPose.getSyncedPoseLocalSpace();
	//nextTransforms = tempTransforms;

	bool tDontExport = false;
#ifdef _ENGINE_DEBUG
	bool bad;
	// CHECK FUCKING EVERYTHING AGAIN
	bad = false;
	for ( int i = 0; i < tempTransforms.getSize(); ++i ) {
		if ( !tempTransforms[i].isOk() ) {
			bad = true;
			tempTransforms[i] = hkQsTransform();
		}
	}
#ifdef _ENGINE_SAFE_CHECK_
	if ( bad ) cout << "BAD SHIT ON TT 2 " << endl;// << i << endl;
#endif
	bad = false;
	for ( int i = 0; i < nextTransforms.getSize(); ++i ) {
		if ( !nextTransforms[i].isOk() ) {
			//cout << "BAD SHIT ON NT 2 " << i << endl;
			bad = true;
			tDontExport = true;
			nextTransforms[i] = hkQsTransform();
		}
	}
#ifdef _ENGINE_SAFE_CHECK_
	if ( bad ) cout << "BAD SHIT ON NT 2 " << endl;// << i << endl;
#endif
	bad = false;
	for ( int i = 0; i < aimrTransforms.getSize(); ++i ) {
		if ( !aimrTransforms[i].isOk() ) {
			//cout << "BAD SHIT ON AM 2 " << i << endl;
			bad = true;
			aimrTransforms[i] = hkQsTransform();
		}
	}
#ifdef _ENGINE_SAFE_CHECK_
	if ( bad ) cout << "BAD SHIT ON AM 2 " << endl;// << i << endl;
#endif
#endif

	// Create the pose
	//hkaPose skeletonPose ( hkaPose::LOCAL_SPACE, mSkelly, nextTransforms );
	// We convert it into model space
	/*hkArray<hkQsTransform> poseModelSpace(mSkelly->m_referenceCount);
	poseModelSpace.setSize(mSkelly->m_referenceCount);
	for ( int i = 0; i < poseModelSpace.getSize(); ++i ) {
		hkaSkeletonUtils::getModelSpaceScale( *mSkelly, nextTransforms.begin(), i, poseModelSpace[i].m_scale );
	}
	hkaSkeletonUtils::transformLocalPoseToModelPose(mSkelly->m_referenceCount, mSkelly->m_parentIndices.begin(), nextTransforms.begin(), poseModelSpace.begin());

	// Do IK
	for ( uint i = 0; i < ikList.size(); ++i )
	{
		if ( bHasAimr && (ikList[i].type == IK_AIMING) )
		{
			// Do AIMING IK
			//hkaPose aimrPose ( hkaPose::LOCAL_SPACE, mSkelly, aimrTransforms );
			hkArray<hkQsTransform> poseAimrModelSpace(mSkelly->m_referenceCount);
			poseAimrModelSpace.setSize(mSkelly->m_referenceCount);
			hkaSkeletonUtils::transformLocalPoseToModelPose(mSkelly->m_referenceCount, mSkelly->m_parentIndices.begin(), aimrTransforms.begin(), poseAimrModelSpace.begin());

			tempTransforms = nextTransforms;

			for ( uint bb = 0; bb < 5; ++bb )
			{
				cout << "BB: " << bb << " : " << ikList[i].bone[bb] << endl;
				//hkQsTransform tempModelAimrTrans = aimrPose.getSyncedPoseModelSpace()[ikList[i].bone[bb]];
				hkQsTransform tempModelAimrTrans = poseAimrModelSpace[ikList[i].bone[bb]];

				//hkQsTransform tempModelNextTrans = skeletonPose.getSyncedPoseModelSpace()[ikList[i].bone[bb]];
				hkQsTransform tempModelNextTrans = poseModelSpace[ikList[i].bone[bb]];
				tempModelNextTrans.setInverse( poseModelSpace[ikList[i].bone[bb]] );
				//tempModelNextTrans.fastRenormalize();

				hkQsTransform tempNextMul;
				//tempNextMul.setMulInverseMul( tempModelNextTrans, tempModelAimrTrans );
				tempNextMul.setMul( tempModelAimrTrans,tempModelNextTrans );
				//skeletonPose.setBoneModelSpace( ikList[i].bone[bb], tempModelTrans, hkaPose::DONT_PROPAGATE );

				hkQsTransform result;
				result.setMul( nextTransforms[ikList[i].bone[bb]], tempNextMul );
				tempTransforms[ikList[i].bone[bb]] = result;
				//nextTransforms[ikList[i].bone[bb]].setMul( nextTransforms[ikList[i].bone[bb]], tempNextMul );
			}

			nextTransforms = tempTransforms;
		}
	}*/


	// Now export all the animations
	//pAnimationSet->Export( animRefs );
	if ( !tDontExport )
	{
		for ( int i = 0; i < nextTransforms.getSize(); ++i )
		{
			//mhkAnim->
			/*int t = mAnimBinding->m_transformTrackToBoneIndices[i];
			((XTransform*)animRefs[t])->position = Vector3d(
				nextTransforms[i].m_translation.getComponent<0>(),
				nextTransforms[i].m_translation.getComponent<1>(),
				nextTransforms[i].m_translation.getComponent<2>() );
			((XTransform*)animRefs[t])->rotation = Quaternion(
				nextTransforms[i].m_rotation.m_vec.getComponent<0>(),
				nextTransforms[i].m_rotation.m_vec.getComponent<1>(),
				nextTransforms[i].m_rotation.m_vec.getComponent<2>(),
				nextTransforms[i].m_rotation.m_vec.getComponent<3>() );
			((XTransform*)animRefs[t])->scale = Vector3d(
				nextTransforms[i].m_scale.getComponent<0>(),
				nextTransforms[i].m_scale.getComponent<1>(),
				nextTransforms[i].m_scale.getComponent<2>() );*/
			int16_t t_indexer = mAnimBinding->findTrackIndexFromBoneIndex(i);
			((XTransform*)animRefs[i])->position = Vector3d(
				nextTransforms[t_indexer].m_translation.getComponent<0>(),
				nextTransforms[t_indexer].m_translation.getComponent<1>(),
				nextTransforms[t_indexer].m_translation.getComponent<2>() );
			((XTransform*)animRefs[i])->rotation = Quaternion(
				nextTransforms[t_indexer].m_rotation.m_vec.getComponent<0>(),
				nextTransforms[t_indexer].m_rotation.m_vec.getComponent<1>(),
				nextTransforms[t_indexer].m_rotation.m_vec.getComponent<2>(),
				nextTransforms[t_indexer].m_rotation.m_vec.getComponent<3>() );
			((XTransform*)animRefs[i])->scale = Vector3d(
				nextTransforms[t_indexer].m_scale.getComponent<0>(),
				nextTransforms[t_indexer].m_scale.getComponent<1>(),
				nextTransforms[t_indexer].m_scale.getComponent<2>() );
		}
	}
}


void	CHKAnimation::SetupMirrorMode ( void )
{
	std::vector<hkaAnimation*>* mAnims = ((CHKAnimationSet*)pAnimationSet)->GetHKAnimation();
	hkaAnimation* mhkAnim = mAnims->at(0);
	if ( mMirrorSkelly == NULL )
	{
		mMirrorSkelly = new hkaMirroredSkeleton( mSkelly );

		// First setup bone mapping
		hkArray<hkStringPtr> ltags;
		ltags.pushBack( "Bip001 L" );
		hkArray<hkStringPtr> rtags;
		rtags.pushBack( "Bip001 R" );
		mMirrorSkelly->computeBonePairingFromNames(ltags,rtags);

		// Second, set up mirroring axis
		mMirrorSkelly->setAllBoneInvariantsFromReferencePose( hkQuaternion(1.0f,0,0,0), 0.01f );
		/*hkArray<hkQsTransform> tempTransforms;
		tempTransforms.setSize( mSkelly->m_bones.getSize() );
		tempTransforms = mSkelly->m_referencePose;
		hkArray<hkReal> tempReals;
		tempReals.setSize( mSkelly->m_bones.getSize() );
		mhkAnim->sampleTracks( 0, tempTransforms.begin(), tempReals.begin() );
		mMirrorSkelly->setAllBoneInvariantsFromSymmetricPose( hkQuaternion(1,0,0,0), 0.01f, tempTransforms.begin() );*/
	}
	if ( mMirrorAnims.empty() )
	{
		//mMirrorAnim = new hkaMirroredAnimation ( mhkAnim, mAnimBinding, mMirrorSkelly );
		//mAnimBinding = mMirrorAnim->createMirroredBinding();
		for ( uint i = 0; i < mAnims->size(); ++i )
		{
			hkaMirroredAnimation* t_mirrorAnim = new hkaMirroredAnimation ( mAnims->at(i), mAnimBinding, mMirrorSkelly );
			//if ( mAnimBinding == NULL )
			if ( i == 0 )
			{
				mAnimBinding = t_mirrorAnim->createMirroredBinding();
			}
			mMirrorAnims.push_back( t_mirrorAnim );
		}

		//for ( int i = 0; i < mSkelly->m_bones.getSize(); ++i )
		//{
		//	int t = mAnimBinding->m_transformTrackToBoneIndices[i];
		//	cout << t << endl;
		//}
	}
}