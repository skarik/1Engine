
#include "RobotAutomaton.h"

#include "core/math/random/Random.h"

#include "core-ext/animation/CAnimAction.h"
#include "core-ext/animation/CAnimation.h"

#include "engine/physics/motion/CRigidbody.h"
#include "engine/physics/collider/types/CCapsuleCollider.h"
#include "engine/physics/raycast/Raycaster.h"

#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/debug/CDebugDrawer.h"

#include "after/physics/Caster.h"
#include "after/entities/character/CRfCharacterModel.h"
#include "after/types/Terrain/BlockType.h"

using namespace NPC;

REGISTER_ZCC(RobotAutomaton);

RobotAutomaton::CONSTRUCTOR_ZCC_V2(RobotAutomaton)
{	ZCC_AddInstance();
	fadeOutValue = 3.0f;

	// Init stats
	stats.iLevel = 0;
	stats.iStrength		= 3; //40+4x
	stats.iAgility		= 50; //20+2x
	stats.iIntelligence	= 2; //5+3x
	stats.ResetStats();
	strcpy( charName, "Automaton" );

	// Init collision
	ai.hull.height = 7.8f;
	ai.hull.radius = 1.5f;

	pCollision = new CCapsuleCollider ( ai.hull.height, ai.hull.radius );
	pRigidBody = NULL;

	// Init model
	//pCharModel = new CRfCharacterModel( this );
	//pCharModel->LoadModel( "fauna/drakes/komodo_lesser_drake" );
	pCharModel = new CCharacterModel( this );
	pCharModel->LoadModel( "robot/automaton" );
	pCharModel->CreateHitboxCollision();
	// add IDLE sequence
	pCharModel->AddSequence( ".res/sequences/automaton_test.seq", "idle", 0, true );
	pCharModel->AddSequence( ".res/sequences/automaton_test_walk.seq", "walk", 0, true );
	pCharModel->AddSequence( ".res/sequences/automaton_test_run.seq", "run", 0, true );
	model = pCharModel;
	
	// Setup AI
	ai.owner = this;
	ai.rigidbody = pRigidBody;

	ai.info_combat.meleeRange = 8.3f;

	ai.info_lookat.lookAway = false;
	ai.info_lookat.glanceAway = false;

	ai.info_lookat.eyeTurnSpeed = 30.0f;
	ai.info_lookat.headTurnSpeed = 1.0f;

	//ai.SetAggroType( NPC::AGGRO_CHARACTER );
	ai.SetAggroType( NPC::AGGRO_MONSTER );

	// set AI move values (these will control animation speed)
	fWalkSpeed = 14.04f;
	fRunSpeed = 14.04f;
	// Init movestuff
	bIsStunned = false;
	fStunTimer = 0;

	// Make a Komodo a wanderer
	//ai.SetFocus( NPC::AIFOCUS_Wanderer );
	ai.SetFocus( NPC::AIFOCUS_None );

	// Now mark AI as ready
	ai.routine_alert = "stand_alert";
	//ai.routine_angry = "human_melee_angry";
	ai.routine_angry = "automaton_melee_angry";

	ai.info_aggro.alertAggro = -100;

	// Now mark AI as ready
	ai.Initialize();
}

RobotAutomaton::~RobotAutomaton ( void )
{
	if ( pRigidBody )
		delete pRigidBody;
	pRigidBody = NULL;
	if ( pCollision )
		delete pCollision;
	pCollision = NULL;
}

void RobotAutomaton::OnDeath ( Damage const& dmg )
{
	ai.SetIsDead( true );

	ai.RequestInfoState( ai_think_state_t::AI_RELAXED );

	ai.SetIsBusy( true );

	ai.GetMoveFeedback().isWalking	= false;
	ai.GetMoveFeedback().isRunning	= false;

	CAnimation* anim = pCharModel->GetAnimationState();
	CAnimAction* death;
	if ( death = anim->FindAction( "death" ) ) {
		/*if ( !bite->isPlaying ) {
			anim->Play( "bite" );
			return true;
		}*/
		death->end_behavior = 1;
		death->layer = 6;
		anim->Play( "death" );
	}
	{
		pCharModel->BlendToRagdoll( Random.Range( 0.6f, 2.6f ) );
	}
}

void RobotAutomaton::OnDamaged ( Damage const& hitDamage, DamageFeedback* dmgFeedback )
{
	if ( Random.Chance(hitDamage.stagger_chance) )
	{
		// Do AI stun
		CAnimation* anim = pCharModel->GetAnimationState();
		CAnimAction* stagger;
		if ( stagger = anim->FindAction( "stagger" ) ) {
			if ( !stagger->isPlaying ) {
				anim->Play( "stagger" );
			
				bIsStunned = true;
				fStunTimer = (stagger->GetLength() / 25.0f);
			}
		}
	}

	ai.OnGetDamaged( hitDamage );

	//cout << "Komodo took " << hitDamage.amount << " damage from " << (ftype)charHealth;
	CCharacter::OnDamaged( hitDamage, dmgFeedback );
	//cout << " to " << (ftype)charHealth << endl;
}

void RobotAutomaton::OnInteract ( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		ai.RequestActorInteract( (CCharacter*)interactingActor, false );
	}
	else if ( interactingActor->IsCharacter() && false )
	{
		ai.RequestActorInteract( (CCharacter*)interactingActor, false );
	}
}

bool RobotAutomaton::PerformAttack ( void )
{
	CAnimation* anim = GetCharModel()->GetAnimationState();
	CAnimAction* bite;
	if ( bite = anim->FindAction( "bite" ) ) {
		if ( !bite->isPlaying ) {
			anim->Play( "bite" );
			return true;
		}
	}
	return false;
}

CCharacter*	RobotAutomaton::GetCombatTarget ( void )
{
	return ai.QueryAggroTarget();
}


void RobotAutomaton::UpdateActive ( void )
{
	if ( !pRigidBody )
	{
		pRigidBody = new CRigidBody ( pCollision, this, 30 );
		pRigidBody->SetRotationEnabled( false );
		pRigidBody->SetQualityType( HK_COLLIDABLE_QUALITY_CHARACTER );
		pRigidBody->SetRestitution( 0.01f );
		pRigidBody->SetCollisionLayer( Layers::PHYS_SWEPTCOLLISION, 1 );
		ai.rigidbody = pRigidBody;
	}
	else {
		//pRigidBody->SetRotation( transform.rotation.getQuaternion() ); // Set hull to face
	}

	// Work the stun timer
	if ( bIsStunned )
	{
		fStunTimer -= Time::deltaTime;
		if ( fStunTimer <= 0 ) {
			ai.RequestStunPanic();
			bIsStunned = false;
		}
	}

	ai.SetIsStunned( bIsStunned );
	ai.SetIsDead( !IsAlive() );

	// Update AI
	ai.Think();

	if ( pCharModel )
	{
		if ( IsAlive() )
		{
			pCharModel->SetSplitFacing( false );
			if ( !bIsStunned ) {
				pCharModel->SetFaceAtPosition( ai.QueryFacingPosition() );
			}
			pCharModel->SetLookAtSpeed( ai.info_lookat.headTurnSpeed, ai.info_lookat.eyeTurnSpeed );
			pCharModel->SetLookAtPosition( ai.QueryLookatPosition() );
			pCharModel->SetEyeRotation( Rotator( ai.QueryEyeGlanceAngles() ) );

			/*pCharModel->SetFaceAtPosition( ai.QueryFacingPosition() );
			pCharModel->SetLookAtPosition( ai.QueryLookatPosition() );
			pCharModel->SetEyeRotation( Rotator( ai.QueryEyeGlanceAngles() ) );*/
			//pCharModel->SetLookAtPosition( CCamera::activeCamera->transform.position );
	
			// Play animations
			MvtDefault();
			/*if (( ai.ai_move.isWalking || ai.ai_move.isRunning )&&( pRigidBody->GetVelocity().sqrMagnitude() > sqr(ai.ai_move.walkSpeed*0.6f) ))
			{
				pCharModel->SetMoveAnimation( "walk" );
				pCharModel->SetMoveAnimationSpeed( 2.2f );
			}
			else {
				pCharModel->SetMoveAnimation( "idle" );
				pCharModel->SetMoveAnimationSpeed( 1.0f );
			}*/
		}
		else
		{
			fadeOutValue -= Time::deltaTime * 0.5f;
			if ( fadeOutValue < -0.5f ) {
				fadeOutValue = -0.5f;
				DeleteObject( this );
			}
		}

		pCharModel->SetAnimationMotion( pRigidBody->GetVelocity() );
		pCharModel->SetModelAlpha( fadeOutValue );

		if ( IsAlive() )
		{
			// Set isBusy state
			CAnimation* anim = pCharModel->GetAnimationState();
			if ( anim )
			{
				ai.SetIsBusy( (*anim)["bite"].isPlaying || (*anim)["stagger"].isPlaying );
			}
		}
	}
	/*
#ifdef _ENGINE_DEBUG
	for ( uint i = 1; i < ai_move.path.size(); ++i ) {
		Debug::Drawer->DrawLine( ai_move.path[i-1], ai_move.path[i] );
	}
#endif
	*/
	MoveUnstuck();
}

void RobotAutomaton::UpdateInactive ( void )
{
	if ( pRigidBody )
	{
		delete pRigidBody;
		pRigidBody = NULL;
		ai.rigidbody = NULL;
	}
}

void RobotAutomaton::OnAnimationEvent ( const Animation::eAnimSystemEvent eventType, const Animation::tag_t tag )
{
	if ( eventType == Animation::Event_Attack )
	{
		// Do attack!
		CActor* result = FivePointCollision();
		/*ai.ai_think.state.waitOnAttackResult = false;
		if ( !result ) {
			ai.ai_think.state.attackResultHit = false;
		}
		else {
			ai.ai_think.state.attackResultHit = true;
		}*/
		ai.response_atk.atk_waitForResult = false;
		if ( !result )
		{
			ai.response_atk.atk_hit = false;
		}
		else
		{
			ai.response_atk.atk_hit = true;
		}
	}
}
// -----------------------------------------------
// Melee Attack Code
// -----------------------------------------------
#include "after/entities/item/CWeaponItem.h"
CActor* RobotAutomaton::FivePointCollision ( void )
{
	Vector3d vrotWeaponArc;
	vrotWeaponArc = GetAimingArc();

	// We want 5 different vectors, then
	Vector3d vWeaponRay [5];
	{
		for ( char i = 0; i < 5; ++i )
			vWeaponRay[i] = Vector3d( 1,0,0 );

		ftype rotFinalArc = vrotWeaponArc.x;
		// Rotate each vector by the needed arc
		vWeaponRay[0] = Quaternion( Vector3d( 0,0,-rotFinalArc/2 ) ) * vWeaponRay[0];
		vWeaponRay[1] = Quaternion( Vector3d( 0,0,-rotFinalArc/4 ) ) * vWeaponRay[1];
		vWeaponRay[3] = Quaternion( Vector3d( 0,0,+rotFinalArc/2 ) ) * vWeaponRay[3];
		vWeaponRay[4] = Quaternion( Vector3d( 0,0,+rotFinalArc/4 ) ) * vWeaponRay[4];

		// Now, rotate all vectors by the general direction
		for ( char i = 0; i < 5; ++i )
			vWeaponRay[i] = Quaternion( Vector3d( vrotWeaponArc.z, 0,0 ) ) * vWeaponRay[i];
	}

	// Currently, we now have the weapon rays
	// We now want actual rays from it.
	/*Ray vHitrays [5];
	{
		for ( char i = 0; i < 5; ++i )
		{
			vHitrays[i].pos = pOwner->GetEyeRay().pos;
			vHitrays[i].dir = pOwner->GetAimRotator() * vWeaponRay[i];
		}
	}*/
	// Initialize the hitlist (most weapons cannot hit an enemy more than once)
	std::vector<CCharacter*> vHitlist;

	const ftype armLength = 1.1f;

	// Loop through all the traces
	for ( char i = 0; i < 5; ++i )
	{
		// Create weapon ray
		Ray vHitray;
		vHitray.pos = GetEyeRay().pos;
		vHitray.dir = GetAimRotator() * vWeaponRay[i];

		RaycastHit result;
		CGameBehavior*	hitBehavior;
		// Do a raytrace for every point in the 5-point check
		//if ( Raytracer.Raycast( vHitray, 1.7f+armLength, &result, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), this ) )
		Item::HitType hittype = Caster::Raycast( vHitray, 1.7f+armLength, &result, &hitBehavior, NULL, 0, this );
		if ( hittype != Item::HIT_NONE && hittype != Item::HIT_UNKNOWN )
		{
			//if (( !CVoxelTerrain::terrainList.empty() )&&( result.pHitBehavior == CVoxelTerrain::terrainList[0] ))
			if ( hittype == Item::HIT_CHARACTER )
			{
				CCharacter* pHitCharacter = (CCharacter*)hitBehavior;
				if ( pHitCharacter != this )
				{
					// Check if the character has been hit already
					if ( find( vHitlist.begin(), vHitlist.end(), pHitCharacter ) == vHitlist.end() )
					{
						// Add character to hit list
						vHitlist.push_back( pHitCharacter );
						// Perform damage to the character
						Damage meleeDamage;
						meleeDamage.amount	= 11.0f;
						meleeDamage.type	= DamageType::Slash;
						meleeDamage.actor	= this;
						meleeDamage.stagger_chance = 0.7f;
						pHitCharacter->OnDamaged( meleeDamage );

						std::cout << "Slashed a "
							<< pHitCharacter->GetTypeName() << " ("
							<< pHitCharacter->GetBaseClassName() << ") "
							<< " for " << 11.0f << " damage."
							<< std::endl;
					}
				}
			}
			else
			{
				std::cout << "Sliced a "
					<< hitBehavior->GetTypeName() << " ("
					<< hitBehavior->GetBaseClassName() << ") "
					<< std::endl;
			}
		}
	}	

	if ( vHitlist.empty() )
		return NULL;
	else
		return vHitlist[0];
}


//#include "../Zones.h"
// Move Unstuck
void RobotAutomaton::MoveUnstuck ( void )
{
	ushort block = Zones.GetBlockAtPosition(transform.position + Vector3d(0,0,0.05f)).block;
	if (( block == Terrain::EB_NONE )||( block == Terrain::EB_WATER ))
	{

	}
	else
	{
		// Issues when z is negative, still
		transform.position.z += 2 - (transform.position.z - floor( transform.position.z / 2 )*2.0f) + 0.02f;
		if ( pRigidBody ) {
			transform.SetDirty();
			pRigidBody->SetVelocity( pRigidBody->GetVelocity() * 0.5f );
		}
	}
}



Ray		RobotAutomaton::GetEyeRay ( void )
{
	if ( !pCharModel ) {
		return CZonedCharacter::GetEyeRay();
	}

	Ray newRay;
	XTransform transCamPos;
	pCharModel->GetEyecamTransform( transCamPos );

	newRay.pos = transCamPos.position;
	newRay.dir = (ai.QueryFacingPosition()-transCamPos.position).normal();

	return newRay;
}

// Get aiming direction. Used for melee weapons.
Rotator RobotAutomaton::GetAimRotator ( void )
{
	//return transform.rotation;
	Quaternion rot;
	Ray eye = GetEyeRay();
	//rot.RotationTo( Vector3d(0,0,0), (ai_lookat.lookatPos-eye.pos)
	//rot.RotationTo( Vector3d(0,0,0), eye.dir );
	rot.RotationTo( Vector3d::forward, (ai.QueryLookatPosition()-eye.pos).normal() );
	//cout << rot << endl;
	Rotator result ( rot );
	//cout << result;
	return Rotator( rot );
	//return transform.rotation;
}


void RobotAutomaton::MvtDefault ( void )
{
	auto ai_moves = ai.GetMoveFeedback();

	ftype targetMoveSpeed;
	if ( ai_moves.isRunning ) {
		targetMoveSpeed = fRunSpeed;
	}
	else if ( ai_moves.isWalking ) {
		targetMoveSpeed = fWalkSpeed;
	}
	else {
		targetMoveSpeed = 0.0f;
	}

	// Get target velocity
	Vector3d moveVector (0,0,0);
	if ( ai_moves.isRunning || ai_moves.isWalking )
	{
		moveVector = ai_moves.final_target - transform.position;
		if ( moveVector.sqrMagnitude() < 0.1f ) {
			ai_moves.isRunning = false; // turn off movement if at target
			ai_moves.isWalking = false;
			moveVector = Vector3d::zero;
		}
		else {
			moveVector.z = 0;
			moveVector.normalize();
		}
	}
	

	// Get velocity
	Vector3d vMoveVelocity = pRigidBody->GetVelocity();
	vMoveVelocity.z = 0;
	// Move with acceleration
	vMoveVelocity += moveVector * Time::deltaTime;
	// Slow down movement
	if ( moveVector.sqrMagnitude() < 0.1f ) {
		vMoveVelocity += (-vMoveVelocity.normal()) * Time::deltaTime * 55.0f;
		if ( vMoveVelocity.magnitude() < Time::deltaTime * 55.0f * 1.2f ) {
			vMoveVelocity = Vector3d( 0,0,0 );
		}
	}
	// Add to move velocity
	else {
		vMoveVelocity += moveVector * Time::deltaTime * 55.0f;
	}
	// Limit velocity
	if ( vMoveVelocity.magnitude() > targetMoveSpeed ) {
		vMoveVelocity = vMoveVelocity.normal() * targetMoveSpeed;
	}
	// Grap proper z movement
	vMoveVelocity.z = pRigidBody->GetVelocity().z;

	// Do blockhopping
	if ( ai_moves.isRunning || ai_moves.isWalking )
	{
		Ray castRay;
		RaycastHit hitInfo;
		// Check for steps
		castRay.pos = transform.position + Vector3d( 0,0,0.1f );
		castRay.dir = moveVector.normal();
		if ( Raycaster.Raycast( castRay, ai.hull.radius*2.0f, &hitInfo, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ) )
		{
			ftype lastDist = hitInfo.distance; // Save old distance
			castRay.pos.z += 2.0f;
			Raycaster.Raycast( castRay, ai.hull.radius*2.0f, &hitInfo, Physics::GetCollisionFilter( Layers::PHYS_WORLD_TRACE ) ); // Cast again at higher position
			if ( !hitInfo.hit || ( hitInfo.distance-2.0f > lastDist ) ) { // If higher position didn't hit, or there's a place to stand
				// We should step up
				vMoveVelocity.z = 3.2f; // So manually set velocity
			}
		}
	}

	// Set final velocity
	pRigidBody->SetVelocity( vMoveVelocity );

	// Set model animations
	Vector3d testCase ( vMoveVelocity.x, vMoveVelocity.y, 0 );
	ftype actualHorizMoveSpeed = testCase.magnitude();
	if ( actualHorizMoveSpeed > fRunSpeed*0.8f ) {
		pCharModel->SetMoveAnimation( "run" );
		//pCharModel->SetMoveAnimationSpeed( 2.5f );
		pCharModel->SetMoveAnimationSpeed( actualHorizMoveSpeed/fRunSpeed );
	}
	else if ( actualHorizMoveSpeed > fWalkSpeed*0.8f ) {
		pCharModel->SetMoveAnimation( "walk" );
		//pCharModel->SetMoveAnimationSpeed( 1.8f );
		pCharModel->SetMoveAnimationSpeed( actualHorizMoveSpeed/fWalkSpeed );
	}
	else {
		pCharModel->SetMoveAnimation( "idle" );
		pCharModel->SetMoveAnimationSpeed( 1.0f );
	}
}