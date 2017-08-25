
#include "CCharacterModel.h"

#include "core-ext/animation/CAnimation.h"
#include "physical/animation/CHKAnimation.h"
#include "physical/skeleton/skeletonBone.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"
//#include "CCharacter.h"
#include "engine-common/entities/CActor.h"

//#include "CHKAnimation.h"
#include "engine-common/physics/motion/CRagdollCollision.h"

#include "physical/physics/CPhysics.h"
#include "engine/physics/raycast/Raycaster.h"


#include "core/math/Math.h"
#include "renderer/debug/CDebugDrawer.h"

#include "after/entities/character/CCharacter.h"

CCharacterModel::CCharacterModel ( CCharacter* owner )
	: CGameObject(), actor( owner ), bShowModel( true )
{
	charModel = NULL;
	charRagdoll = NULL;

	sCurrentMoveAnim	= "idle";
	vMotionVelocity		= Vector3d::zero;
	vPlacementOffset	= Vector3d::zero;

	iFallType = 0;
	iIdleType = LockNone;
	bSplitFacing = false;
	
	pEyeL	= NULL;
	pEyeR	= NULL;
	pHead	= NULL;
	pNeck	= NULL;
	pSpine0 = NULL;
	pSpine1	= NULL;
	pSpine2 = NULL;
	pSpine3 = NULL;
	pUpperArmL = NULL;
	pUpperArmR = NULL;
	pProp1	= NULL;
	pProp2	= NULL;
	pProp3	= NULL;
	pProp4	= NULL;
	pFootL0	= NULL;
	pFootL1	= NULL;
	pFootR0	= NULL;
	pFootR1	= NULL;

	// face at and lookats
	faceatRotation = Rotator();
	lookatRotation = Rotator();

	faceatRotationFrom = Rotator();
	bTurnToFace = true;
	fTurnToTimer = 1;

	lookatHeadTarget	= Vector3d();
	lookatHeadPosition	= Vector3d();
	lookatHeadPositionS	= Vector3d();
	lookatEyePosition	= Vector3d();
	lookatEyePositionS	= Vector3d();
	lookatHeadTimer = 0;
	lookatEyeTimer = 0;
	
	faceatPosRotation	= Rotator();
	faceatPosRotationTarget	= Rotator();
	faceatPosTimer		= 0;

	// lookat settings
	lookatHeadSpeed		= 2.0f;
	lookatEyeSpeed		= 8.0f;

	// ik settings
	m_IKFootEnable		= true;

	// ragdoll settings
	fRagdollTargetStrength = 0;
	fRagdollStrength = 0;
	fRagdollBlendSpeed = 1;

	// 
	fModelAlpha = 1.0f;

	//
	fAnimCooldown = 0;

	// Set variable information
	bPerformSequenceInterrupts = true;
	currentSequence = NULL;

	// default settings
	useDirectionalMovementAnimation = true;
}

CCharacterModel::~CCharacterModel ( void )
{
	//cout << "Entering ~CCharacterModel" << endl;
	//delete_safe( charModel );
	delete_safe( charModel );
	charTargetModel = NULL;
	delete_safe_decrement( charRagdoll );
}

bool CCharacterModel::LoadBase( const string& character_name )
{
	basename = character_name;

	//string target_filetempname = basename;
	//SetBaseModel( target_filetempname );

	pEyeL	= NULL;
	pEyeR	= NULL;
	pHead	= NULL;
	pNeck	= NULL;
	pSpine0 = NULL;
	pSpine1	= NULL;
	pSpine2 = NULL;
	pSpine3 = NULL;
	pUpperArmL = NULL;
	pUpperArmR = NULL;
	pProp1	= NULL;
	pProp2	= NULL;


	string file = "models/character/" + character_name + ".fbx";

	charModel = new CSkinnedModel ( file );
	charTargetModel = charModel;

	//ConfigureAnimations();

	Load();

	return true;
}
void CCharacterModel::ConfigureBase ( CSkinnedModel* target_model )
{
	pEyeL	= NULL;
	pEyeR	= NULL;
	pHead	= NULL;
	pNeck	= NULL;
	pSpine0 = NULL;
	pSpine1	= NULL;
	pSpine2 = NULL;
	pSpine3 = NULL;
	pUpperArmL = NULL;
	pUpperArmR = NULL;
	pProp1	= NULL;
	pProp2	= NULL;


	charModel = target_model;
	charTargetModel = charModel;

	ConfigureAnimations();
}

bool CCharacterModel::LoadModel ( const string& model_filename )
{
	basename = "";

	string file = "models/" + model_filename + ".fbx";

	charModel = new CSkinnedModel ( file );
	charTargetModel = charModel;

	ConfigureCommonAnimations();

	return true;
}

void CCharacterModel::Update ( void )
{
	if ( !charModel ) {
		return;
	}
	charModel->UpdateSkeleton();

	if ( actor ) {
		//charModel->transform.Get( character->transform );
		transform.position = actor->transform.position;
		charModel->transform.position = transform.position + vPlacementOffset;
		//charModel->UpdateSkeleton();
	}
	else {
		charModel->transform.position = transform.position;
	}
	fAnimCooldown -= Time::deltaTime;
	if ( !bIsIdling ) {
		PlayAnimation( sCurrentMoveAnim );
	}
	else {
		PlayAnimationIdle( sCurrentMoveAnim );
	}
	UpdateAnimations();

	//charModel->visible = bShowModel;
	//charModel->visible = false;
	charModel->SetVisibility( bShowModel );
}

void CCharacterModel::CreateHitboxCollision ( void )
{
	// Create ragdoll
	if ( !charRagdoll ) {
		charRagdoll = new CRagdollCollision( charModel );
		charRagdoll->SetActor( actor );
	}
}

void CCharacterModel::RemoveAnimationEvent ( const Animation::eAnimSystemEvent& nTypeToDelete )
{
	auto it = animEventList.begin();
	while ( it != animEventList.end() )
	{
		if ( (*it).type == nTypeToDelete ) {
			it = animEventList.erase(it);
		}
		else {
			++it;
		}
	}
}
void CCharacterModel::LateUpdate ( void ) 
{
	// Need to grab the events, and then send them to the character.
	{
		unsigned int animEventCount;
		charModel->GetAnimation()->GetEvents( animEventList, animEventCount );
		if ( animEventCount > 0 )
		{
			/*for ( unsigned int i = 0; i < animEventCount; ++i )
			{
				if ( character ) {
					//cout << "Signalling event: " << animEventList[i] << endl;
					character->OnAnimationEvent( animEventList[i] ); // Send event to the character
				}
				// Check if there was a change in the event list
				if ( animEventList.size() != animEventCount ) {
					i += animEventCount - animEventList.size();
					animEventCount = animEventList.size();
				}
			}*/
			//
			while ( !animEventList.empty() ) {
				if ( actor ) {
					Animation::ActionEvent currentEvent = animEventList[0];
					std::cout << "EVENT: " << currentEvent.type << " DATA: " << currentEvent.data << std::endl; // DEBUG OUTPUT INFO
					actor->OnAnimationEvent( currentEvent.type, currentEvent.data );
					animEventList.erase( animEventList.begin() );
				}
			}
			//
		}
	}

	{
		// Raytrace from center origin to eyes
		XTransform eyeTransform;
		GetEyecamTransform( eyeTransform );
		Vector3d eyePos = eyeTransform.position;
		
		Ray rEyeCheck;
		rEyeCheck.pos = Vector3d( transform.position.x, transform.position.y, eyePos.z );
		rEyeCheck.dir = ( eyePos - rEyeCheck.pos );
		ftype fEyeCheckDist = rEyeCheck.dir.magnitude() + 0.3f;
		rEyeCheck.dir.normalize();
		RaycastHit rhHitResult;

		// If trace hits the environment, then pull back the character model so that it's not in the environment
		if ( Raycaster.Raycast( rEyeCheck, fEyeCheckDist, &rhHitResult, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE), actor ) )
		{
			Vector3d offset = rEyeCheck.dir * ( fEyeCheckDist - rhHitResult.distance );
			charModel->transform.position -= offset;
			//charModel->GetSkeletonRoot()->position += Quaternion::CreateRotationTo(rEyeCheck.dir,Vector3d(0,1,0)) * offset;
			//transform.position -= rEyeCheck.dir * ( fEyeCheckDist - rhHitResult.distance );
			actor->transform.position -= offset;
			actor->transform.SetDirty();
			//charModel->UpdateSkeleton();
		}
	}

	// Foot IK
	if ( charModel )
	{
		// Set the IK info
		CAnimation* anim = charModel->GetAnimation();
		ikinfo_t& ik_lf = anim->GetIKInfo( "def_lfoot" );
		//ik_lf.subinfo[0] = 0;
		ik_lf.subinfo[1] = m_IKFootEnable ? 1.0f : 0.0f;
		ik_lf.enabled = (fRagdollStrength < 0.5f);
		ikinfo_t& ik_rf = anim->GetIKInfo( "def_rfoot" );
		//ik_rf.subinfo[0] = 0;
		ik_rf.subinfo[1] = m_IKFootEnable ? 1.0f : 0.0f;
		ik_rf.enabled = (fRagdollStrength < 0.5f);
	}
	m_IKFootEnable = true;
}

// Interface to control ragdolling
void CCharacterModel::BlendToRagdoll ( ftype time )
{
	fRagdollTargetStrength = 1;
	if ( time < FTYPE_PRECISION ) {
		fRagdollBlendSpeed = 100000;
	}
	else {
		fRagdollBlendSpeed = 1/time;
	}
}
void CCharacterModel::BlendToAnimation ( ftype time )
{
	fRagdollTargetStrength = 0;
	if ( time < FTYPE_PRECISION ) {
		fRagdollBlendSpeed = 100000;
	}
	else {
		fRagdollBlendSpeed = 1/time;
	}
}
void CCharacterModel::PostUpdate ( void )
{
	ftype delta = Math.sgn(fRagdollTargetStrength-fRagdollStrength) * Time::deltaTime * fRagdollBlendSpeed;
	if ( fabs(fRagdollTargetStrength-fRagdollStrength) > fabs(delta) ) {
		fRagdollStrength += delta;
	}
	else {
		fRagdollStrength = fRagdollTargetStrength;
	}
	//
	// Loop through model's bones and set the ragdoll params
	ftype calcRagdollStrength;
	calcRagdollStrength = powf( fRagdollStrength, 0.022f );
	//calcRagdollStrength = powf( fRagdollStrength, 0.011f );
	//cout << "calcRagdollStrength: " << calcRagdollStrength << endl;
	for ( uint i = 0; i < charModel->GetSkeletonList()->size(); ++i )
	{
		skeletonBone_t* bone = charModel->GetSkeletonList()->at(i);
		bone->ragdollStrength = calcRagdollStrength;
	}

	// If ragdoll strength all the way up, stop animations
	if ( fRagdollStrength > 1-FTYPE_PRECISION )
	{
		sCurrentMoveAnim = "ref";
	}
}
void CCharacterModel::PostFixedUpdate ( void )
{
	//
	charModel->UpdateSkeleton();
}

const CSkinnedModel* CCharacterModel::GetModelLowLevel ( void )
{
	return charModel;
}
CAnimation* CCharacterModel::GetAnimationState ( void )
{
	if ( charModel ) {
		return charModel->GetAnimation();
	}
	return NULL;
}
Transform* CCharacterModel::GetSkeletonRoot ( void )
{
	if ( charModel ) {
		return charModel->GetSkeletonRoot();
	}
	return NULL;
}
const Rotator&	CCharacterModel::GetModelRotation ( void )
{
	if ( charModel ) {
		return charModel->transform.rotation;
	}
	return transform.rotation;
}


// Rotation setters
void CCharacterModel::SetFaceAtRotation ( const Rotator& rotation )
{
	if ( !bSplitFacing )
	{
		faceatRotation = rotation;
		charModel->transform.rotation = faceatRotation;
		bTurnToFace = true;
		if ( CAnimation::useHavok )
		{
			try
			{
				// Now, get the anim, cast to HK anim, and configure the IK
				CAnimation* anim = charModel->GetAnimation();
				// Set the strength
				ikinfo_t& ik0 = anim->GetIKInfo( "def_aimer0" );
				ik0.subinput1.y = 0;
			}
			catch ( std::out_of_range& )
			{
				;
			}
		}
	}
	else
	{
		Real facingDifference = (faceatRotation * Vector3d::forward).dot( rotation * Vector3d::forward );
		Real facingDifferenceFrom = (faceatRotationFrom * Vector3d::forward).dot( rotation * Vector3d::forward );
		if ( iIdleType == 2 )
		{
			bTurnToFace = true;
		}
		else if (( vMotionVelocity.magnitude() > 4.0f )||( facingDifference < 0.7f ))
		{
			if ( bTurnToFace == false ) {
				fTurnToTimer = 0;
			}
			bTurnToFace = true;
			//faceatRotation = rotation;
			faceatRotationFrom = faceatRotation;
		}

		if ( bTurnToFace )
		{
			fTurnToTimer += Time::deltaTime * ( 3.0f / (4-facingDifferenceFrom*3) );
			//faceatRotation = faceatRotationFrom.LerpTo( rotation, Math.Clamp( fTurnToTimer,0,1 ) );
			Quaternion qfaceatRotationFrom = faceatRotationFrom.getQuaternion();
			Quaternion qrotation = rotation.getQuaternion();
			Quaternion qfaceatRotation = qfaceatRotationFrom.Slerp( qrotation, Math.Clamp( fTurnToTimer,0,1 ) );
			qfaceatRotation.Normalize();
			faceatRotation = qfaceatRotation;
			if ( fTurnToTimer >= 1 ) {
				bTurnToFace = false;
			}
		}

		charModel->transform.rotation = faceatRotation;
		Rotator difference = rotation * faceatRotation.inverse();

		Vector3d angle ( difference.getEulerAngles().z,0,0 );
		if ( !CAnimation::useHavok )
		{
			if ( !pSpine1 )
				pSpine1 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine1" );
			if ( !pSpine2 )
				pSpine2 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine2" );
			if ( !pSpine3 )
				pSpine3 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine3" );

			pSpine1->localRotation.Euler( pSpine1->localRotation.getEulerAngles() + angle*0.444f );
			pSpine2->localRotation.Euler( pSpine2->localRotation.getEulerAngles() + angle*0.333f );
			pSpine3->localRotation.Euler( pSpine3->localRotation.getEulerAngles() + angle*0.222f );
		}
		else
		{
			try
			{
				// Now, get the anim, cast to HK anim, and configure the IK
				CAnimation* anim = charModel->GetAnimation();
				// Set the strength
				ikinfo_t& ik0 = anim->GetIKInfo( "def_aimer0" );
				ik0.subinput1.y = angle.x;
			}
			catch ( std::out_of_range& )
			{
				if ( !pSpine1 )
					pSpine1 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine1" );
				if ( !pSpine2 )
					pSpine2 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine2" );
				if ( !pSpine3 )
					pSpine3 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine3" );

				pSpine1->localRotation.Euler( pSpine1->localRotation.getEulerAngles() + angle*0.444f );
				pSpine2->localRotation.Euler( pSpine2->localRotation.getEulerAngles() + angle*0.333f );
				pSpine3->localRotation.Euler( pSpine3->localRotation.getEulerAngles() + angle*0.222f );
			}
		}
	}
}
//#include "CDebugDrawer.h"
void CCharacterModel::SetLookAtPosition ( const Vector3d& vLookAtPos )
{
	if ( !pHead ) {
		pHead = charModel->GetSkeletonRoot()->FindChildRecursive( "Head" );
	}

	if ( pHead )
	{
		if ( !CAnimation::useHavok )
		{
			// Need angle to where camera looking
			Quaternion rot;
			Quaternion rotFlat;
			//rot.RotationTo( charModel->transform.Forward(), (vLookAtPos-charModel->transform.position).normal() );
			//rot.RotationTo( (charModel->transform.rotation*Rotator(0,0,90))*Vector3d::forward, (vLookAtPos-charModel->transform.position).normal() );
			XTransform eyecamPos;
			GetEyecamTransform( eyecamPos );
			//rot.RotationTo( Vector3d(0,1,0)*!charModel->transform.rotation*Rotator(eyecamPos.rotation), (vLookAtPos-eyecamPos.position).normal() );
			//rot.RotationTo( Rotator(eyecamPos.rotation)*charModel->transform.rotation*Vector3d(0,1,0), (vLookAtPos-eyecamPos.position).normal() );//*!
			//rot.RotationTo( charModel->transform.rotation*(eyecamPos.rotation*Vector3d(0,1,0)), (vLookAtPos-eyecamPos.position).normal() );
			Vector3d lookAtOffsetLocal = (!charModel->transform.rotation)*(vLookAtPos-eyecamPos.position).normal();
			Vector3d lookAtOffsetLocalFlat = Vector3d( lookAtOffsetLocal.x, lookAtOffsetLocal.y, 0 ).normal();
			//eyecamPos.rotation = (Rotator( eyecamPos.rotation ) * charModel->transform.rotation).getQuaternion();

			//target.rotation = pHead->rotation.getQuaternion();
			//target.rotation = ( !charModel->transform.rotation * pHead->rotation ).getQuaternion();
			eyecamPos.rotation = pHead->rotation.getQuaternion();

			Vector3d fromVect = eyecamPos.rotation*Vector3d(0,1,0);
			Vector3d fromVectFlat = Vector3d( fromVect.x, fromVect.y, 0 ).normal();

			rot.RotationTo( fromVect, lookAtOffsetLocal );
			rotFlat.RotationTo( fromVectFlat, lookAtOffsetLocalFlat );

			//DebugD::DrawLine( charModel->transform.position, charModel->transform.position+charModel->transform.Forward() );
			//DebugD::DrawLine( eyecamPos.position, eyecamPos.position+lookAtOffsetLocal );
			//DebugD::DrawLine( eyecamPos.position, eyecamPos.position +  charModel->transform.rotation*(eyecamPos.rotation*Vector3d(0,1,0)) );
			//DebugD::DrawLine( eyecamPos.position, eyecamPos.position + (!charModel->transform.rotation)*Vector3d::forward );
			//DebugD::DrawLine( eyecamPos.position, eyecamPos.position + (!charModel->transform.rotation)*Vector3d::up );
			//DebugD::DrawLine( eyecamPos.position, eyecamPos.position + (!charModel->transform.rotation)*Vector3d::left );

			//Vector3d lookAtOffsetLocal = (vLookAtPos-eyecamPos.position).normal();;
			//rot.RotationTo( charModel->transform.rotation*Rotator(eyecamPos.rotation)*Vector3d(0,-1,0), lookAtOffsetLocal );
		
			//pHead->localRotation.Euler( pHead->localRotation.getEulerAngles() + angle*0.4f );
			//rot = rot * Quaternion( Vector3d( 0,90,0 ) );
			//Vector3d euler = Rotator(rot).getEulerAngles();
			Vector3d euler = (Rotator(rot).getEulerAngles());
			Vector3d euler_flat = (Rotator(rot).getEulerAngles());

			euler_flat.z = (ftype)radtodeg( atan2( (fromVectFlat.cross(lookAtOffsetLocalFlat)).magnitude(), fromVectFlat.dot(lookAtOffsetLocalFlat) ) ); // sum gay ass shit here

			euler.z = euler_flat.z * Math.sgn<ftype>( euler.z );
			//euler = euler_flat;
			//cout << euler << endl;

			//pHead->localRotation *= Rotator(rot);
			// HEAVENS HELP US LIMIT THE NECK TURNING
			if ( euler.z > 180 ) {
				euler.z -= 360;
			}
			if ( fabs(euler.z) > 75 ) {
				euler.x *= 1.0f/std::max<ftype>( (fabs(euler.z)-75)*0.05f, 1.0f ); // Decrease other angles the further the angle
				euler.y *= 1.0f/std::max<ftype>( (fabs(euler.z)-75)*0.1f, 1.0f );
				if ( euler.z > 85 ) {
					euler.z = 85;
				}
				else if ( euler.z < -85 ) {
					euler.z = -85;
				}
			}

			// Slightly reduce Z axis head turn
			euler.z *= 0.96f;

			Rotator targetLookatRotation = Rotator( Vector3d( euler.z, euler.y, euler.x ) );
			//Rotator deltaRotation = lookatRotation*(targetLookatRotation*lookatRotation.inverse())

			Vector3d deltaEuler = targetLookatRotation.getEulerAngles() - lookatRotation.getEulerAngles();
			if ( fabs(deltaEuler.x) >= 180 ) {
				deltaEuler.x -= Math.sgn<ftype>(deltaEuler.x) * 360;
			}
			if ( fabs(deltaEuler.y) >= 180 ) {
				deltaEuler.y -= Math.sgn<ftype>(deltaEuler.y) * 360;
			}
			if ( fabs(deltaEuler.z) >= 180 ) {
				deltaEuler.z -= Math.sgn<ftype>(deltaEuler.z) * 360;
			}
			//ftype maxSpeed = (130-std::max<ftype>(0,90-deltaEuler.sqrMagnitude()*0.5f)) * Time::deltaTime * 2.0f; // slow down when close to target
			ftype maxSpeed = (110-std::max<ftype>(0,90-deltaEuler.sqrMagnitude()*0.5f)) * Time::deltaTime * 2.0f * 0.8f; // slow down when close to target
			if ( deltaEuler.magnitude() > maxSpeed ) {	// Limit turn speed
				deltaEuler = deltaEuler.normal() * maxSpeed;
			}
			lookatRotation.Euler( lookatRotation.getEulerAngles() + deltaEuler );

			euler = lookatRotation.getEulerAngles();
			if ( fabs(euler.x) >= 180 ) { //These are to help with the euler.magnitude line
				euler.x -= Math.sgn<ftype>(euler.x) * 360;
			}
			if ( fabs(euler.y) >= 180 ) {
				euler.y -= Math.sgn<ftype>(euler.y) * 360;
			}
			if ( fabs(euler.z) >= 180 ) {
				euler.z -= Math.sgn<ftype>(euler.z) * 360;
			}
			if ( euler.magnitude() > 95 ) {	// Limit angle size
				euler = euler.normal() * 95;
				lookatRotation.Euler( euler );
			}


			pHead->localRotation *= lookatRotation;

			CAnimation* anim = charModel->GetAnimation();
			ikinfo_t& ik0 = anim->GetIKInfo( "def_lookat" );
			ik0.enabled = false;
		}
		else
		{
			// Now, get the anim, cast to HK anim, and configure the IK
			CAnimation* anim = charModel->GetAnimation();

			//pHead->localRotation = lookatRotation;
			//Vector3d eyePosition = charModel->transform.rotation*((pEyeL->position + pEyeR->position)*0.5f + (pHead->rotation.getQuaternion()*Vector3d( 0,0.19f,0 ))) + charModel->transform.position;
			Vector3d eyePosition = Vector3d(0,0,0);//charModel->transform.rotation*((pEyeL->position + pEyeR->position)*0.5f) + charModel->transform.position;

			// Move the position to the target
			Vector3d delta;
			ftype distance;
			{	// Head rotation
				// Generate target
				distance = (lookatHeadTarget-vLookAtPos).magnitude();
				if ( distance > 0.5f ) { 
					lookatHeadTarget = vLookAtPos;
				}
				else if ( distance < 0.1f ) {
					lookatHeadTarget = lookatHeadTarget.lerp( vLookAtPos, 0.2f );
				}
				// Generate delta
				delta = lookatHeadPositionS-vLookAtPos;
				distance = delta.magnitude();
				// If at target position and moved, then reset the starting position.
				if ( lookatHeadTimer >= 1.0f - FTYPE_PRECISION )
				{
					if ( distance > 30 )
					{
						lookatHeadPosition = vLookAtPos + delta.normal() * 30;
						lookatHeadTimer = 0;
					}
					else if ( distance > 0.1f ) 
					{
						lookatHeadPosition = lookatHeadPositionS;
						lookatHeadTimer = 0;
					}
				}
				// Lerp towards the target
				lookatHeadTimer = Math.Clamp( lookatHeadTimer + Time::deltaTime * lookatHeadSpeed, 0, 1 );
				lookatHeadPositionS = lookatHeadPosition.lerp( vLookAtPos, lookatHeadTimer );
				// Do not let the lookat position get too close to the head
				Vector3d headDelta = (lookatHeadPositionS-eyePosition);
				Real targetDistance = headDelta.magnitude();
				const Real minimumDistance = 2.1f;
				if ( targetDistance < minimumDistance ) {
					lookatHeadPositionS = eyePosition + (headDelta/targetDistance)*minimumDistance;
				}
			}
			{	//  Eye rotation
				delta = lookatEyePositionS-vLookAtPos;
				distance = delta.magnitude();
				// If at target position and moved, then reset the starting position.
				if ( lookatEyeTimer >= 1.0f - FTYPE_PRECISION )
				{
					if ( distance > 15 )
					{
						lookatEyePosition = vLookAtPos + delta.normal() * 15;
						lookatEyeTimer = 0;
					}
					else if ( distance > 0.1f ) 
					{
						lookatEyePosition = lookatEyePositionS;
						lookatEyeTimer = 0;
					}
				}
				// Lerp towards the target
				lookatEyeTimer = Math.Clamp( lookatEyeTimer + Time::deltaTime * lookatEyeSpeed, 0, 1 );
				lookatEyePositionS = lookatEyePosition.lerp( vLookAtPos, lookatEyeTimer );
				// Do not let the lookat position get too close to the head
				Vector3d headDelta = (lookatEyePositionS-eyePosition);
				Real targetDistance = headDelta.magnitude();
				const Real minimumDistance = 0.6f;
				if ( targetDistance < minimumDistance ) {
					lookatEyePositionS = eyePosition + (headDelta/targetDistance)*minimumDistance;
				}
			}

			// Set the IK info
			ikinfo_t& ik0 = anim->GetIKInfo( "def_lookat" );
			ik0.input = lookatHeadPositionS;//!charModel->transform.rotation * (lookatHeadPositionS - charModel->transform.position);//charModel->GetSkeletonList()->at(ik0.bone[0])->transform.position;
			ik0.subinput0 = lookatEyePositionS;//!charModel->transform.rotation * (lookatEyePositionS - charModel->transform.position);
			ik0.enabled = true;
			//Rotator* value = &lookatRotation;
			//memcpy( &(ik0.subinfo[0]), &value, sizeof(Rotator*) );
		}
	}

	return;
}

void CCharacterModel::SetFaceAtPosition ( const Vector3d& vFaceAtPos )
{
	// Need angle to where camera looking
	Quaternion rot;
	XTransform eyecamPos;
	GetEyecamTransform( eyecamPos );
	//rot.RotationTo( eyecamPos.rotation * Vector3d(0,1,0), (vFaceAtPos-eyecamPos.position).normal() );

	//Vector3d planarOffset = (vFaceAtPos-eyecamPos.position);
	Vector3d planarOffset = (vFaceAtPos-transform.position);
	planarOffset.z = 0;
	if ( planarOffset.sqrMagnitude() < 0.003f ) {
		planarOffset = Vector3d::forward;
	}
	rot.RotationTo( transform.rotation*Vector3d(0,-1,0), planarOffset.normal() );

	//DebugD::DrawLine( charModel->transform.position, charModel->transform.position + charModel->transform.rotation * Vector3d::forward * 2 );

	Vector3d euler = Rotator(rot).getEulerAngles();

	Rotator targetFaceatRotation = Rotator( Vector3d( 0,0,euler.z ) ) ;
	/*Rotator deltaRotation = targetFaceatRotation * faceatRotation;
	//Rotator deltaRotation = targetFaceatRotation;
	euler = deltaRotation.getEulerAngles();
	cout << "deltaRotation: " << deltaRotation.getEulerAngles() << endl;
	cout << "faceatRotation: " << faceatRotation.getEulerAngles() << endl;
	cout << "targetFaceatRotation: " << targetFaceatRotation.getEulerAngles() << endl;
	faceatRotation = faceatRotation * deltaRotation;
	cout << "faceatRotation result: " << faceatRotation.getEulerAngles() << endl;*/

	Vector3d deltaEuler = targetFaceatRotation.getEulerAngles() - faceatRotation.getEulerAngles();
	if ( fabs(deltaEuler.x) >= 180 ) {
		deltaEuler.x -= Math.sgn<ftype>(deltaEuler.x) * 360;
	}
	if ( fabs(deltaEuler.y) >= 180 ) {
		deltaEuler.y -= Math.sgn<ftype>(deltaEuler.y) * 360;
	}
	if ( fabs(deltaEuler.z) >= 180 ) {
		deltaEuler.z -= Math.sgn<ftype>(deltaEuler.z) * 360;
	}
	if ( deltaEuler.magnitude() > (130 * Time::deltaTime) ) {	// Limit turn speed
		deltaEuler = deltaEuler.normal() * 130 * Time::deltaTime;
	}
	//faceatRotation.Euler( faceatRotation.getEulerAngles() + deltaEuler );
	/*Rotator targetRotation;
	targetRotation.Euler( faceatRotation.getEulerAngles() + deltaEuler );*/
	Rotator targetRotation;// = targetFaceatRotation;
	//targetRotation.Lerp( targetFaceatRotation, 0.3f );
	if ( (faceatPosRotationTarget * Vector3d::forward).dot( targetFaceatRotation * Vector3d::forward ) < 0.97f )
	{
		faceatPosTimer = 0;
		faceatPosRotation = faceatRotation;
		faceatPosRotationTarget = targetFaceatRotation;
	}

	faceatPosTimer += Time::deltaTime;
	if ( faceatPosTimer < 1 ) {
		targetRotation = faceatPosRotation;
		targetRotation.Euler( faceatRotation.getEulerAngles() + deltaEuler );
		targetRotation.Lerp( targetFaceatRotation, faceatPosTimer );
	}
	else {
		targetRotation = targetFaceatRotation;
	}

	SetFaceAtRotation( targetRotation );
}

void CCharacterModel::SetEyeRotation ( const Rotator& eyes )
{
	if ( !pEyeL )
		pEyeL = charModel->GetSkeletonRoot()->FindChildRecursive( "L Eye" );
	if ( !pEyeR )
		pEyeR = charModel->GetSkeletonRoot()->FindChildRecursive( "R Eye" );

	if ( pEyeL && pEyeR ) {
		pEyeL->localRotation *= eyes;
		pEyeR->localRotation *= eyes;
	}
}

void CCharacterModel::SetLookAtSpeed ( const float headSpeed, const float eyeSpeed )
{
	lookatHeadSpeed = headSpeed;
	lookatEyeSpeed = eyeSpeed;
}

void CCharacterModel::FixAimingAnglesHack ( const Rotator & aimRotator )
{
	Debug::Console->PrintWarning( "FixAimingAnglesHack is deprecated\n" );

	if ( !pHead )
		pHead = charModel->GetSkeletonRoot()->FindChildRecursive( "Head" );
	if ( !pSpine1 )
		pSpine1 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine1" );
	if ( !pSpine0 )
		pSpine0 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine" );
	if ( !pSpine2 )
		pSpine2 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine2" );
	if ( !pSpine3 )
		pSpine3 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine3" );
	if ( !pUpperArmL )
		pUpperArmL = charModel->GetSkeletonRoot()->FindChildRecursive( "L UpperArm" );
	if ( !pUpperArmR )
		pUpperArmR = charModel->GetSkeletonRoot()->FindChildRecursive( "R UpperArm" );

	if ( pHead && pSpine1 && pSpine0 )
	{
		//Vector3d angle ( pHead->rotation.getEulerAngles().x,0, 0 );

		//pSpine1->localRotation.Euler( pSpine1->localRotation.getEulerAngles() - angle );
		/*Quaternion angle;
		angle.CreateRotationTo( Vector3d(1,0,0), Vector3d(1,0,0)*pHead->rotation );

		pSpine1->localRotation *= Rotator( angle );*/

		Vector3d target;
		target = pHead->rotation * Vector3d( 1,0,0 );
		target.y = target.x;
		target.x = target.z;
		//cout << "TARGET: " << target << endl;
		target.z = 0;
		Quaternion angle;
		angle.RotationTo( Vector3d(1,0,0), target );

		Vector3d targetAngle = angle.GetEulerAngles();
		//cout << targetAngle << endl;

		Vector3d finalAngle = Vector3d( targetAngle.z*0.5f, 0,0 );

		pSpine0->localRotation.Euler( pSpine0->localRotation.getEulerAngles() + finalAngle );
		pSpine1->localRotation.Euler( pSpine1->localRotation.getEulerAngles() + finalAngle*1.3f );
		pSpine2->localRotation.Euler( pSpine2->localRotation.getEulerAngles() + finalAngle*1.5f );
		pSpine3->localRotation.Euler( pSpine3->localRotation.getEulerAngles() + finalAngle*1.8f );

		Vector3d armAngle = Vector3d( 0, -targetAngle.z*0.1f, -aimRotator.getEulerAngles().y*0.2f );

		pUpperArmR->localRotation.Euler( pUpperArmR->localRotation.getEulerAngles() + armAngle );
		pUpperArmL->localRotation.Euler( pUpperArmL->localRotation.getEulerAngles() + armAngle );
	}
}

void CCharacterModel::SetLookAtRotations ( const Vector3d & headEuler, const Rotator & torso )
{
	if ( !pHead )
		pHead = charModel->GetSkeletonRoot()->FindChildRecursive( "Head" );
	if ( !pSpine1 )
		pSpine1 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine1" );
	if ( !pSpine2 )
		pSpine2 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine2" );
	if ( !pSpine3 )
		pSpine3 = charModel->GetSkeletonRoot()->FindChildRecursive( "Spine3" );
	if ( !pNeck )
		pNeck = charModel->GetSkeletonRoot()->FindChildRecursive( "Neck" );
	
	if ( pHead && pSpine1 )
	{
		//Vector3d angle ( head.getEulerAngles().z,0,head.getEulerAngles().y );
		Vector3d angle( headEuler.z, 0, headEuler.y );
		if ( angle.z < -180 )
			angle.z += 360;
		else if ( angle.z > 180 )
			angle.z -= 360;

		if ( !CAnimation::useHavok )
		{
			switch ( iIdleType )
			{
			case LockNone:
				pHead->localRotation.Euler( pHead->localRotation.getEulerAngles() + angle*0.5f );
				pSpine1->localRotation.Euler( pSpine1->localRotation.getEulerAngles() + angle*0.15f );
				pSpine2->localRotation.Euler( pSpine2->localRotation.getEulerAngles() + angle*0.10f );
				pSpine3->localRotation.Euler( pSpine3->localRotation.getEulerAngles() + angle*0.05f );
				pNeck->localRotation.Euler( pNeck->localRotation.getEulerAngles() + angle*0.20f );
				break;
			case LockMajor:
				pHead->localRotation.Euler( pHead->localRotation.getEulerAngles() + angle*0.4f );
				pSpine1->localRotation.Euler( pSpine1->localRotation.getEulerAngles() + angle*0.1f );
				pSpine2->localRotation.Euler( pSpine2->localRotation.getEulerAngles() + angle*0.05f );
				//pSpine3->localRotation.Euler( pSpine3->localRotation.getEulerAngles() + angle*0.0f );
				pNeck->localRotation.Euler( pNeck->localRotation.getEulerAngles() + angle*0.2f );
				break;
			case LockAllButNeck:
				pHead->localRotation.Euler( pHead->localRotation.getEulerAngles() + angle*0.6f );
				pNeck->localRotation.Euler( pNeck->localRotation.getEulerAngles() + angle*0.1f );
				break;
			case LockAllButHead:
				pHead->localRotation.Euler( pHead->localRotation.getEulerAngles() + angle*0.5f );
				break;
			}
		}
		else
		{
			// Now, get the anim, cast to HK anim, and configure the IK
			CAnimation* anim = charModel->GetAnimation();
			// Set the strength
			ikinfo_t& ik0 = anim->GetIKInfo( "def_aimer0" );
			ik0.input = angle;

			switch ( iIdleType )
			{
			case LockNone:
				ik0.subinput0.x = 0.5f;		// Head
				ik0.subinput0.y = 0.15f;	// Spine
				ik0.subinput0.z = 0.05f;	// Spine degrade as go up tree
				ik0.subinput1.x = 0.20f;	// Neck
				break;
			case LockMajor:
				ik0.subinput0.x = 0.4f;		// Head
				ik0.subinput0.y = 0.10f;	// Spine
				ik0.subinput0.z = 0.05f;	// Spine degrade as go up tree
				ik0.subinput1.x = 0.20f;	// Neck
				break;
			case LockAllButNeck:
				ik0.subinput0.x = 0.6f;		// Head
				ik0.subinput0.y = 0.00f;	// Spine
				ik0.subinput0.z = 0.05f;	// Spine degrade as go up tree
				ik0.subinput1.x = 0.10f;	// Neck
				break;
			case LockAllButHead:
				ik0.subinput0.x = 0.5f;		// Head
				ik0.subinput0.y = 0.00f;	// Spine
				ik0.subinput0.z = 0.05f;	// Spine degrade as go up tree
				ik0.subinput1.x = 0.00f;	// Neck
				break;
			}
		}

		// Feed the rotation into the IK
		if ( iIdleType != LockAll )
		{
			// Now, get the anim, cast to HK anim, and configure the IK
			CAnimation* anim = charModel->GetAnimation();
	
			// Set the strength
			ikinfo_t& ik0 = anim->GetIKInfo( "def_aimer0" );
			ik0.input = angle;
		}

		// Turn off head IK
		CAnimation* anim = charModel->GetAnimation();
		ikinfo_t& ik0 = anim->GetIKInfo( "def_lookat" );
		ik0.enabled = false;
	}
}

// === Transforms ===
void CCharacterModel::GetEyecamTransform ( XTransform& target )
{
	// Left and Right eye transforms both need to be grabbed (and head)
	if ( !pEyeL )
		pEyeL = charModel->GetSkeletonRoot()->FindChildRecursive( "L Eye" ); 
	if ( !pEyeR )
		pEyeR = charModel->GetSkeletonRoot()->FindChildRecursive( "R Eye" ); 
	if ( !pHead )
		pHead = charModel->GetSkeletonRoot()->FindChildRecursive( "Head" ); 

	if ( (!pEyeL) || (!pEyeR) || (!pHead) )
	{
		if ( !pEyeL )
			std::cout << "CAN NOT FIND LEFT EYE TRANSFORM.\n";
		if ( !pEyeR )
			std::cout << "CAN NOT FIND RIGHT EYE TRANSFORM.\n";
		if ( !pHead )
			std::cout << "CAN NOT FIND HEAD TRANSFORM.\n";
	}
	else
	{
		//target.rotation = (leftEye->rotation.lerp( rightEye->rotation, 0.5f )).getQuaternion();
		//target.scale = leftEye->scale.lerp( rightEye->scale, 0.5f );
		//target.position = leftEye->position + rightEye->position;
		
		//target.position *= target.scale;
		/*target.position.x *= target.scale.x;
		target.position.y *= target.scale.y;
		target.position.z *= target.scale.z;
		target.position = target.rotation*target.position;*/

		target.rotation = pHead->rotation.getQuaternion();
		target.position = charModel->transform.rotation*((pEyeL->position + pEyeR->position)*0.5f + (target.rotation*Vector3d( 0,0.19f,0 ))) + charModel->transform.position;

		target.rotation = ( charModel->transform.rotation * pHead->rotation ).getQuaternion();
	}
}
void CCharacterModel::GetHeadTransform ( XTransform &target )
{
	if ( !pHead )
		pHead = charModel->GetSkeletonRoot()->FindChildRecursive( "Head" ); 

	if ( !pHead )
	{
		if ( !pHead )
			std::cout << "CAN NOT FIND HEAD TRANSFORM.\n";
	}
	else
	{
		target.position = charModel->transform.rotation*pHead->position + charModel->transform.position;
		target.rotation = ( charModel->transform.rotation * pHead->rotation ).getQuaternion();
	}
}


void CCharacterModel::GetProp01Transform ( XTransform& target )
{
	if ( !pProp1 )
		pProp1 = charModel->GetSkeletonRoot()->FindChildRecursive( "Prop1" );

	if ( pProp1 )
	{
		target.position = charModel->transform.rotation*pProp1->position + charModel->transform.position;
		///target.rotation = prop->rotation.getQuaternion()*charModel->transform.rotation.getQuaternion();
		///target.rotation = !((charModel->transform.rotation*!prop->rotation).getQuaternion());
		//target.rotation = ( charModel->transform.rotation * prop->rotation ).getQuaternion();
		//target.rotation = ( pProp1->rotation * charModel->transform.rotation ).getQuaternion();
		target.rotation = ( charModel->transform.rotation * pProp1->rotation ).getQuaternion();
	}
}
void CCharacterModel::GetProp02Transform ( XTransform& target )
{
	if ( !pProp2 )
		pProp2 = charModel->GetSkeletonRoot()->FindChildRecursive( "Prop2" );

	if ( pProp2 )
	{
		target.position = charModel->transform.rotation*pProp2->position + charModel->transform.position;
		///target.rotation = prop->rotation.getQuaternion()*charModel->transform.rotation.getQuaternion();
		///target.rotation = !((charModel->transform.rotation*!prop->rotation).getQuaternion());
		//target.rotation = ( charModel->transform.rotation * prop->rotation ).getQuaternion();
		target.rotation = ( charModel->transform.rotation * pProp2->rotation ).getQuaternion();
	}
}
void CCharacterModel::GetLFootTransform ( const int index, XTransform & target )
{
	if ( !pFootL0 )
		pFootL0 = charModel->GetSkeletonRoot()->FindChildRecursive( "L Foot" );

	if ( pFootL0 )
	{
		target.position = charModel->transform.rotation*pFootL0->position + charModel->transform.position;
		target.rotation = ( charModel->transform.rotation * pFootL0->rotation ).getQuaternion();
	}
}
void CCharacterModel::GetRFootTransform ( const int index, XTransform & target )
{
	if ( !pFootR0 )
		pFootR0 = charModel->GetSkeletonRoot()->FindChildRecursive( "R Foot" );

	if ( pFootL0 )
	{
		target.position = charModel->transform.rotation*pFootR0->position + charModel->transform.position;
		target.rotation = ( charModel->transform.rotation * pFootR0->rotation ).getQuaternion();
	}
}

void CCharacterModel::GetBoneTransform ( const int index, XTransform & target )
{
	Transform* boneTransform = &(charModel->GetSkeletonList()->at(index)->transform);

	if ( boneTransform )
	{
		target.position = charModel->transform.rotation*boneTransform->position + charModel->transform.position;
		target.rotation = ( charModel->transform.rotation * boneTransform->rotation ).getQuaternion();
	}
}


void CCharacterModel::SetVerticalOffset ( const ftype offset )
{
	vPlacementOffset.z = offset;
}


void CCharacterModel::SetSplitFacing ( const bool splitfacing )
{
	bSplitFacing = splitfacing;
}

int CCharacterModel::GetMeleeAttackFrame ( const short hand )
{
	CAnimation& anim = (*(charModel->GetAnimation()));

	// For now, look at the animations

	arstring<256> t_animName;
	//hold_oneblade_01_swing-0 to 8 for oneblade swings
	for ( uint i = 0; i < 9; ++i ) {
		sprintf( t_animName, "hold_oneblade_01_swing-%d", i );
		CAnimAction& action = anim[t_animName.c_str()];
		if ( action.isPlaying ) {
			int midpoint = action.GetEvent( Animation::Event_Attack );
			if ( action.frame >= midpoint ) {
				return (int)(action.frame-midpoint) + 100;
			}
			else {
				return (int)(action.frame);
			}
		}
	}
	
	return -1;
}


// ==== IK Control ====
void CCharacterModel::SetAimerIK ( const uchar nArmIndex, const Item::HoldType& nHoldType )
{
	// Now, get the anim, cast to HK anim, and configure the IK
	CAnimation* anim = charModel->GetAnimation();


	//
	//ik0.subinput0.x = 0.50f;	// Head
	//ik0.subinput0.y = 0.00f;	// Spine
	//ik0.subinput0.z = 0.05f;	// Spine degrade as go up tree
	//ik0.subinput1.x = 0.00f;	// Neck
	//ik0.subinput1.y	// Faceat rotation
	//ik0.subinput1.z = 1.00f;	// Blend values
	//

	//
	// ik.subinfo[3]		// Strength
	// ik.subinfo[0]		// Left arm strength
	// ik.subinfo[1]		// Right arm strength
	// ik.subinfo[2]		// Limit ignore

	Real t_maxStrength = 1-fRagdollStrength;

	// Set the strength of the IK
	if ( nHoldType == Item::EmptyHanded ||
		( sCurrentMoveAnim == "anim" )||( sCurrentMoveAnim == "hang_idle" ) )
	{
		ikinfo_t& ik0 = anim->GetIKInfo( "def_aimer0" );

		// Check for fist actions
		if ( (*anim)["fisticuffs_left"].isPlaying || (*anim)["fisticuffs_left_hit"].isPlaying || (*anim)["fisticuffs_left_hit_broke"].isPlaying )
		{	// Left hand IK
			ik0.subinfo[0] = 1.0f;
			ik0.subinfo[1] = 0.0f;
			ik0.subinfo[2] = 0.0f;
			ik0.subinfo[3] += Time::deltaTime*4;
			ik0.enabled = true;
			if ( ik0.subinfo[3] > t_maxStrength ) ik0.subinfo[3] = t_maxStrength;
		}
		else if ( (*anim)["fisticuffs_right"].isPlaying || (*anim)["fisticuffs_right_hit"].isPlaying || (*anim)["fisticuffs_right_hit_broke"].isPlaying )
		{	// Right hand IK
			ik0.subinfo[0] = 0.0f;
			ik0.subinfo[1] = 1.0f;
			ik0.subinfo[2] = 0.0f;
			ik0.subinfo[3] += Time::deltaTime*4;
			ik0.enabled = true;
			if ( ik0.subinfo[3] > t_maxStrength ) ik0.subinfo[3] = t_maxStrength;
		}
		else
		{	// No actions, turn off the IK
			ik0.subinfo[0] = 0.0f;
			ik0.subinfo[1] = 0.0f;
			ik0.subinfo[2] = 0.0f;
			ik0.subinfo[3] -= Time::deltaTime*4;
			ik0.enabled = true;
			if ( ik0.subinfo[3] < 0 ) ik0.subinfo[3] = 0;
		}
	}
	else
	{	// All other cases, turn on the aimer IK
		ikinfo_t& ik0 = anim->GetIKInfo( "def_aimer0" );
		
		ik0.subinfo[3] += Time::deltaTime*4;
		ik0.enabled = true;
		if ( ik0.subinfo[3] > t_maxStrength ) ik0.subinfo[3] = t_maxStrength;

		switch ( nHoldType ) {
			case Item::TwoHandedAxe:
				ik0.subinfo[0] = 1.0f;
				ik0.subinfo[1] = 1.0f;
				ik0.subinfo[2] = 0.0f;
				break;
			case Item::Bow:
				ik0.subinfo[0] = 1.0f;
				ik0.subinfo[1] = 1.0f;
				ik0.subinfo[2] = 1.0f;
				break;
			case Item::Sword:
			case Item::TwoHandedSword:
				ik0.subinfo[0] = 1.0f;
				ik0.subinfo[1] = 1.0f;
				ik0.subinfo[2] = 0.0f;
				break;
			case Item::Default:
			default:
				ik0.subinfo[0] = 0.0f;
				ik0.subinfo[1] = 1.0f;
				ik0.subinfo[2] = 0.0f;
				break;
		}
	}
}