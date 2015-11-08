
#include "after/entities/item/weapon/CBaseRandomMelee.h"
#include "engine/physics/raycast/Raycaster.h"
#include "after/entities/character/CCharacter.h"
#include "engine-common/entities/effects/CFXMaterialHit.h"
#include "after/entities/item/weapon/CWeaponItemGenerator.h"
#include "core/math/Math.h"
#include "after/entities/character/CAfterPlayer.h"
#include "after/entities/CCharacterModel.h"
#include "after/terrain/edit/CTerrainAccessor.h"
#include "after/physics/Caster.h"
#include "after/interfaces/CCharacterMotion.h"

#include "engine-common/entities/CParticleSystem.h"
#include "renderer/material/glMaterial.h"
#include "after/entities/foliage/CTreeBase.h"
#include "after/entities/props/CTerrainProp.h"

CBaseRandomMelee::CBaseRandomMelee( tMeleeWeaponProperties &newProps, const WItemData &wdat )
	: CBaseRandomItem( wdat ),
	bIsCharging(false), bIsBlocking(false), bIsParrying(false), bIsDashing(false), bEnableQuickstrike(false),
	bShowTrail(false), m_trail_ps(NULL)
{
	base_weapon_stats = newProps;
	weapon_stats = base_weapon_stats;
	weaponItemData.iHands = weapon_stats.hands;

	InitializeBlockDestruction();
	bHardnessSet = false;

	//vHoldingOffset = Vector3d( 0.0f, 0.35f, -0.05f ); // Move hand to the handle
	vHoldingOffset = Vector3d( 0.0f, 0.35f, -0.00f ); // Move hand to the handle
}
CBaseRandomMelee::~CBaseRandomMelee ( void )
{
	delete_safe( m_trail_ps );
}
CWeaponItem& CBaseRandomMelee::operator= ( const CWeaponItem * original )
{
	CBaseRandomMelee* oitem = (CBaseRandomMelee*)original;
	base_weapon_stats	= oitem->base_weapon_stats;
	weapon_stats		= oitem->weapon_stats;
	weaponItemData.iHands = weapon_stats.hands;

	return CBaseRandomItem::operator= ( original );
}

// Serialization
void CBaseRandomMelee::serialize ( Serializer & ser, const uint ver )
{
	CBaseRandomItem::serialize(ser,ver);

	ser & base_weapon_stats.reach;
	ser & base_weapon_stats.damage;
	ser & base_weapon_stats.hands;
	ser & base_weapon_stats.damagetype;
	ser & base_weapon_stats.recover_time;
	ser & base_weapon_stats.charge_full_time;
	ser & base_weapon_stats.charge_min_damage;
	ser & base_weapon_stats.charge_max_damage;
	ser & base_weapon_stats.charge_min_stagger;
	ser & base_weapon_stats.charge_max_stagger;
	ser & base_weapon_stats.stagger_chance;

	weaponItemData.iHands = weapon_stats.hands;
}

void CBaseRandomMelee::Generate ( void )
{
	weapon_stats = base_weapon_stats;
	CBaseRandomItem::Generate();
}

void CBaseRandomMelee::UpdateHoldStation ( void )
{
	Vector4d vrotWeaponArc;
	vrotWeaponArc = pOwner->GetAimingArc();

	fHoldStation = 0;
	Vector2d checkArea ( -(ftype)cos(degtorad(vrotWeaponArc.z)),-(ftype)sin(degtorad(vrotWeaponArc.z)) );

	if ( checkArea.y > 0.924f ) {
		fHoldStation = 4;
	}
	else if ( checkArea.y > 0.383f ) {
		if ( checkArea.x < 0 ) {
			fHoldStation = 5;
		}
		else {
			fHoldStation = 3;
		}
	}
	else if ( checkArea.y > -0.383f ) {
		if ( checkArea.x < 0 ) {
			fHoldStation = 6;
		}
		else {
			fHoldStation = 2;
		}
	}
	else if ( checkArea.y > -0.924f ) {
		if ( checkArea.x < 0 ) {
			fHoldStation = 7;
		}
		else {
			fHoldStation = 1;
		}
	}
	else {
		fHoldStation = 0;
	}
	if ( vrotWeaponArc.w > 0.707f ) {
		fHoldStation = 8;
	}
}

bool CBaseRandomMelee::Use ( int x )
{
	if ( x == Item::USecondary ) {
		x = Item::UDefend;
	}
	if ( !CanUse( x ) ) {
		return true;
	}
	/*if ( x == UPrimary )
	{
		pOwner->PlayItemAnimation( "attack" );

		SetCooldown( x, weapon_stats.speed );
		return true;
	}
	return false;*/
	if ( x == Item::UPrimary )
	{
		bool bCanAttack = true;
		if ( pOwner->GetCharStats() ) {
			if ( pOwner->GetCharStats()->fStamina < weaponItemData.fWeight * 0.5f ) {
				bCanAttack = false;
			}
		}
		if ( bCanAttack )
		{
			UpdateHoldStation();

			if ( bIsCharging )
			{
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Charge, 1, mHand, fHoldStation );
				fChargeTime += Time::deltaTime;
			}
			else if ( !bIsBlocking )
			{
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Charge, 1, mHand, fHoldStation );
				fChargeTime = 0;
				bIsCharging = true;
			}
		}
		return true;
	}
	else if ( x == Item::UDefend )
	{
		if ( bIsBlocking )
		{
			//SetCooldown( UPrimary,		weapon_stats.recover_time*0.7f );
			//SetCooldown( UPrimarySprint,weapon_stats.recover_time*0.5f );
			UpdateHoldStation();

			if ( pOwner ) {
				((CCharacter*)pOwner)->SetCanSprint( false );
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Defend, 1, mHand, fHoldStation );
			}
			//sAnimationIdle = "defend";
			mAnimationIdleAction = NPC::ItemAnim::Defend;
			mAnimationIdleArgs = fHoldStation;
			fBlockTime += Time::deltaTime;

			if ( fBlockTime > 0.35f ) {
				bIsParrying = false;
			}
		}
		/*else if ( !bIsCharging )
		{
			SetCooldown( UPrimary,		weapon_stats.recover_time*0.7f );
			SetCooldown( UPrimarySprint,weapon_stats.recover_time*0.5f );

			((CCharacter*)pOwner)->SetCanSprint( false );
			//sAnimationIdle = "defend";
			mAnimationIdleAction = NPC::ItemAnimDefend;
			pOwner->PlayItemAnimation( NPC::ItemAnimDefend, 1, fHoldStation );

			fBlockTime = 0;

			bIsBlocking = true;
			bIsParrying = true;
		}*/
		return true;
	}
	else if ( x == Item::UPrimarySprint )
	{
		if ( bIsBlocking ) {
			return true;
		}
		bIsCharging = false;
		SetCooldown( x,			weapon_stats.recover_time );
		SetCooldown( Item::UPrimary,	weapon_stats.recover_time );
		SetCooldown( Item::UDefend,	weapon_stats.recover_time );
		
		bool bCanAttack = true;
		if ( pOwner->GetCharStats() ) {
			if ( pOwner->GetCharStats()->fStamina < weaponItemData.fWeight * 0.5f ) {
				bCanAttack = false;
			}
		}
		if ( bCanAttack )
		{
			// Go to dash attack
			//if ( pOwner == CAfterPlayer::GetActivePlayer() ) {
			if ( pOwner->ActorType() == ACTOR_TYPE_PLAYER ) {
				((CAfterPlayer*)pOwner)->GetMotionState()->SetMovementModeQueued( NPC::MOVEMENT_MINIDASH );
			}
			else {
				// Do NPC minidash
			}

			// Grab check
			CCharacter* ownerCharacter = NULL;
			if ( pOwner->IsCharacter() ) {
				ownerCharacter = (CCharacter*)pOwner;
			}

			// Set to dash mode
			bIsDashing = true;
			
			// Show trail effect
			bShowTrail = true;

			UpdateHoldStation();
			// Play attack animation
			if ( bEnableQuickstrike ) {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Quickstrike, 1, mHand, fHoldStation, 0.8f / weapon_stats.recover_time - 1 );
				if ( ownerCharacter ) {
					ownerCharacter->ApplyLinearMotion( Vector3d( 7.0f,0,0 ), 0.1f );
				}
			}
			else {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Attack, 1, mHand, fHoldStation, 0.8f / weapon_stats.recover_time - 1 );
				if ( ownerCharacter ) {
					ownerCharacter->ApplyLinearMotion( Vector3d( 2.0f,0,0 ), 0.25f );
				}
			}
			//ftype chargeAmount = std::min<ftype>( 1.0f, fChargeTime/weapon_stats.charge_full_time );
			ftype chargeAmount = 1.5f;
			/*CPlayer* activePlayer = (CPlayer*) CPlayer::GetActivePlayer();
			if ( pOwner == activePlayer )
			{
				Vector3d vrotWeaponArc;
				vrotWeaponArc = pOwner->GetAimingArc();
				Vector3d punchAmount = Quaternion( Vector3d( vrotWeaponArc.z, 0,0 ) ) * Vector3d(0,0,-weaponItemData.fWeight) * (chargeAmount + 1.0f);
				punchAmount.x = -punchAmount.x;
				activePlayer->PunchView( punchAmount );
			}*/
			// Subtract stamina
			CharacterStats* stats = pOwner->GetCharStats();
			if ( stats ) {
				fStartAttackStamina = stats->fStamina;
				stats->fStamina -= (chargeAmount+0.7f)*weaponItemData.fWeight*2.0f;
			}
		}
	}
	return false;
}
// Activating the item has just started.
void CBaseRandomMelee::StartUse ( int x )
{
	if ( x == Item::USecondary ) {
		x = Item::UDefend;
	}
	if ( !CanUse( x ) ) {
		return;
	}
	if ( x == Item::UPrimary )
	{
		if ( bIsBlocking ) {
			bIsBlocking = false;
		} // Left click cancels block but charges up.
		bIsCharging = true;
		fChargeTime = 0;

		// Update hold state now
		UpdateHoldStation();

		// If player, then update turn sensitivity
		if ( pOwner->ActorType() == ACTOR_TYPE_PLAYER ) {
			((CAfterPlayer*)pOwner)->SetTurnSensitivity( 0.6f );
		}
	}
	else if ( x == Item::UDefend )
	{
		if ( bIsCharging ) {
			bIsCharging = false;
			SetCooldown( Item::UPrimary, weapon_stats.recover_time*0.5f );
		}
		else {
			// Play parry animation
			bIsBlocking = true;
			bIsParrying = true;

			fBlockTime = 0;

			((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Parry, 1, mHand, fHoldStation );

			SetCooldown( Item::UPrimary,		weapon_stats.recover_time*0.7f );
			SetCooldown( Item::UPrimarySprint,weapon_stats.recover_time*0.5f );

			if ( pOwner->ActorType() == ACTOR_TYPE_PLAYER ) {
				((CAfterPlayer*)pOwner)->SetTurnSensitivity( 0.4f );
			}
			if ( pOwner->IsCharacter() ) {
				((CCharacter*)pOwner)->SetMovementSpeedScale( 0.5f ); 
			}
		}
	}
}
// Activating the item has ended.
void CBaseRandomMelee::EndUse ( int x )
{
	if ( x == Item::USecondary ) {
		x = Item::UDefend;
	}
	if ( !CanUse( x ) ) {
		return;
	}
	if ( bIsCharging )
	{
		if ( x == Item::UPrimary )
		{
			CCharacter* ownerCharacter = NULL;
			if ( pOwner->IsCharacter() ) {
				ownerCharacter = (CCharacter*)pOwner;
			}

			bIsCharging = false;
			bIsDashing = false; // Disable dash mode
			SetCooldown( Item::UPrimary,	weapon_stats.recover_time );
			SetCooldown( Item::UDefend,	weapon_stats.recover_time );
			
			// Show trail effect
			bShowTrail = true;

			ftype chargeAmount = std::min<ftype>( 1.0f, fChargeTime/weapon_stats.charge_full_time );

			if ( bEnableQuickstrike ) {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Quickstrike, 1, mHand, fHoldStation, 0.8f / weapon_stats.recover_time - 1, 0.3f-chargeAmount*0.3f );
				if ( ownerCharacter ) {
					ownerCharacter->ApplyLinearMotion( Vector3d( 7.0f,0,0 ), 0.1f );
				}
			}
			else {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Attack, 1, mHand, fHoldStation, 0.8f / weapon_stats.recover_time - 1, 0.3f-chargeAmount*0.3f );
				if ( ownerCharacter ) {
					ownerCharacter->ApplyLinearMotion( Vector3d( 2.0f,0,0 ), 0.25f );
				}
			}

			/*CPlayer* activePlayer = (CPlayer*) CPlayer::GetActivePlayer();
			if ( pOwner == activePlayer )
			{
				Vector3d vrotWeaponArc;
				vrotWeaponArc = pOwner->GetAimingArc();
				Vector3d punchAmount = Quaternion( Vector3d( vrotWeaponArc.z, 0,0 ) ) * Vector3d(0,0,-weaponItemData.fWeight) * (chargeAmount + 1.0f ) ;
				punchAmount.x = -punchAmount.x;
				activePlayer->PunchView( punchAmount );
			}*/
			// Subtract stamina
			CharacterStats* stats = pOwner->GetCharStats();
			if ( stats )
			{
				fStartAttackStamina = stats->fStamina;
				stats->fStamina -= (chargeAmount+0.7f)*weaponItemData.fWeight*2.0f;
			}

			if ( pOwner->ActorType() == ACTOR_TYPE_PLAYER ) {
				((CAfterPlayer*)pOwner)->SetTurnSensitivity();
			}
			if ( pOwner->IsCharacter() ) {
				((CCharacter*)pOwner)->SetMovementSpeedScale(); 
			}
		}
	}
	if ( bIsBlocking )
	{
		if ( x == Item::UDefend )
		{
			bIsBlocking = false;
			if ( bIsParrying ) {
				SetCooldown( Item::UDefend,	weapon_stats.recover_time*0.7f );
			}
			else {
				SetCooldown( Item::UDefend,	weapon_stats.recover_time*0.4f );
			}

			((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::DefendStop, 1, mHand, fHoldStation );

			((CCharacter*)pOwner)->SetCanSprint( true );
			mAnimationIdleAction = NPC::ItemAnim::Idle;

			bIsParrying = false;

			if ( pOwner->ActorType() == ACTOR_TYPE_PLAYER ) {
				((CAfterPlayer*)pOwner)->SetTurnSensitivity();
			}
			if ( pOwner->IsCharacter() ) {
				((CCharacter*)pOwner)->SetMovementSpeedScale(); 
			}
		}
	}
}

void CBaseRandomMelee::OnUnequip ( CActor* interactingActor ) 
{
	((CCharacter*)interactingActor)->SetCanSprint( true );
	//((CCharacter*)interactingActor)->SetViewAngle
	if ( interactingActor->ActorType() == ACTOR_TYPE_PLAYER ) {
		((CAfterPlayer*)interactingActor)->SetTurnSensitivity();
	}
	if ( interactingActor->IsCharacter() ) {
		((CCharacter*)interactingActor)->SetMovementSpeedScale(); 
	}
	mAnimationIdleAction = NPC::ItemAnim::Idle;
	//sAnimationIdle = "idle";
}

// Return stance of the item. Returns StanceDefense when defending.
Item::EquipStance CBaseRandomMelee::GetStance ( void )
{
	if ( bIsBlocking ) {
		return Item::StanceDefense;
	}
	else {
		return Item::StanceOffense;
	}
}

//
void	CBaseRandomMelee::OnBlockAttack ( Damage& resultantDamage )
{
	// Do behavior for blocking
	if ( bIsBlocking ) {
		if ( bIsParrying ) {
			// If parrying, set damage to zero
			resultantDamage.amount = 0;
			// And immediately reenable attacking
			SetCooldown( Item::UPrimary, weapon_stats.recover_time * 0.01f );
			SetCooldown( Item::UPrimarySprint, weapon_stats.recover_time * 0.01f );
			// Go to quickstrike mode
			bEnableQuickstrike = true;
			fQuickstrikeTimer = 0.4f;
		}
		else {
			// When blocking, set attacking cooldown
			SetCooldown( Item::UPrimary, weapon_stats.recover_time );
			SetCooldown( Item::UPrimarySprint, weapon_stats.recover_time*0.6f );
		}
	}
}

void	CBaseRandomMelee::Update ( void )
{
	// TODO: modifiers based on stats : D
	weapon_stats = base_weapon_stats;

	CBaseRandomItem::Update();

	if ( fQuickstrikeTimer > 0 ) {
		fQuickstrikeTimer -= Time::deltaTime;
	}
	else {
		bEnableQuickstrike = false;
	}

	UpdateParticleSystem();

	if ( pBody ) {
		pBody->EnableCollisionCallback();
	}
}

// Sword trail effect
void CBaseRandomMelee::UpdateParticleSystem ( void )
{
	if ( !CanUse( Item::UPrimary ) && holdState == Item::Holding && bShowTrail )
	{
		if ( m_trail_ps == NULL )
		{
			m_trail_ps = new CParticleSystem ( ".res/particlesystems/bladetrail.pcf" );
		}
		m_trail_ps->enabled = true;
		m_trail_ps->transform.position = transform.position + transform.rotation*Vector3d( 0, weapon_stats.reach*0.8f, 0 );
	}
	else
	{
		bShowTrail = false;
		if ( m_trail_ps )
		{
			m_trail_ps->enabled = false;
		}
		m_swingHitlist.clear();
	}
}

// -----------------------------------------------
// Melee Clash Code
// -----------------------------------------------
void CBaseRandomMelee::ClangCheck ( void )
{
	// First, perform arc collision checking
	Vector4d vrotWeaponArc;
	vrotWeaponArc = pOwner->GetAimingArc();
	
	const int rayCount = 13;
	Vector3d vWeaponRay [rayCount];
	{
		for ( char i = 0; i < rayCount; ++i )
			vWeaponRay[i] = Vector3d( 1,0,0 );

		ftype rotFinalArc = vrotWeaponArc.x + (weapon_stats.reach-1.2f)*8.0f;
		if ( vrotWeaponArc.w > 0.6f )
			rotFinalArc *= 0.6f - vrotWeaponArc.w;

		// Rotate each vector by the needed arc
		for ( int i = 0; i < rayCount/2; ++i ) {
			vWeaponRay[(rayCount/2)-i] = Quaternion( Vector3d( 0,0,-(rotFinalArc/(rayCount-1))*(i+1) ) ) * vWeaponRay[(rayCount/2)-i];
			vWeaponRay[(rayCount/2)+i] = Quaternion( Vector3d( 0,0,+(rotFinalArc/(rayCount-1))*(i+1) ) ) * vWeaponRay[(rayCount/2)+i];
		}

		// Now, rotate all vectors by the general direction
		for ( char i = 0; i < rayCount; ++i ) {
			vWeaponRay[i] = Quaternion( Vector3d( vrotWeaponArc.z, 0,0 ) ) * vWeaponRay[i];
		}
	}

	ftype armLength = 1.5f;
	if ( bIsDashing ) armLength += 0.7f;

	// Loop through all the traces
	for ( char i = 0; i < rayCount; ++i )
	{
		// Create weapon ray
		Ray vHitray;
		vHitray.pos = pOwner->GetEyeRay().pos;
		vHitray.dir = pOwner->GetAimRotator() * vWeaponRay[i];

		RaycastHit	result;
		BlockTrackInfo	block;	// keep the block for when hitting other things
		CGameBehavior*	hitBehavior;
		ftype			multiplier;
		// Do a raytrace for every point in the 5-point check
		//if ( Raytracer.Raycast( vHitray, weapon_stats.reach+armLength, &result, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31) ) )
		Item::HitType hittype = Caster::Raycast( vHitray, weapon_stats.reach+armLength, &result, &block, &hitBehavior, &multiplier, 0, pOwner );
		if ( hittype != Item::HIT_NONE && hittype != Item::HIT_UNKNOWN )
		{
			if ( hittype == Item::HIT_ACTOR || hittype == Item::HIT_CHARACTER ) // omg its a character
			{
				// Check for a character object
 				if ( hittype == Item::HIT_CHARACTER ) // omg its REALLY a character
				{
					CCharacter* pHitCharacter = (CCharacter*)hitBehavior;
					if ( pHitCharacter != pOwner )
					{
						// Check owner's frame time and this one's frame time
						int mframe = ((CCharacter*)(pOwner))->GetAttackingFrame(0);
						int oframe = ((CCharacter*)(pHitCharacter))->GetAttackingFrame(0);
						// For sword clashing, check that both people's frame sets are in the 'preattack' swet
						if ( mframe < 100 && mframe >= 0 && oframe < 100 && oframe >= 0 )
						{
							if ( abs( mframe - oframe ) < 10 )
							{	// Frame difference is smaller than 10, do sword clash
								((CCharacter*)(pOwner))->model->RemoveAnimationEvent( Animation::Event_Attack );

								((CCharacter*)(pOwner))->PlayItemAnimation( NPC::ItemAnim::Parry, 1, mHand, fHoldStation );
								pHitCharacter->PlayItemAnimation( NPC::ItemAnim::Parry, 1, mHand, fHoldStation );

								if ( pOwner && pOwner->IsCharacter() ) {
									((CCharacter*)pOwner)->ApplyStun( "hold_oneblade_01_ministun-0-4", 1.0f );
									((CCharacter*)pOwner)->ApplyLinearMotion( Vector3d( -7.0f,0,0 ), 0.1f );
								}
								if ( pHitCharacter && pHitCharacter->IsCharacter() ) {
									((CCharacter*)pHitCharacter)->ApplyStun( "hold_oneblade_01_ministun-0-4", 1.0f );
									((CCharacter*)pHitCharacter)->ApplyLinearMotion( Vector3d( -7.0f,0,0 ), 0.1f );
								}

								// Deal damage to stun both, but make sure the damage ticker is accurate
								Damage meleeDamage;
								meleeDamage.amount	= 0;
								meleeDamage.type	= weapon_stats.damagetype | DamageType::Reflect;
								meleeDamage.stagger_chance = 2.0f;
								meleeDamage.actor	= pOwner;
								meleeDamage.direction = vHitray.dir.normal();
								meleeDamage.source	= transform.position;
								meleeDamage.applyDirection = Vector3d( (ftype)cos(degtorad(vrotWeaponArc.z)),(ftype)sin(degtorad(vrotWeaponArc.z)),vrotWeaponArc.w*1.2f ).normal();
								// And stun both parties
								pOwner->OnDamaged( meleeDamage );
								pHitCharacter->OnDamaged( meleeDamage );

								// Different stun animation?

								// Do material effects
								CFXMaterialHit* newHitEffect = new CFXMaterialHit(
									*PhysMats::Get(PhysMats::MAT_Wood),
									result, CFXMaterialHit::HT_HIT );
								newHitEffect->RemoveReference();
							}
						}
						// Only sword clash first enemy hit. The ray is rotated into the correct direction already, so this return will always be correct.
						return;
					}
				}
			}
		}
	}
}

// -----------------------------------------------
// Melee Attack Code
// -----------------------------------------------
// Collision Callbacks
void CBaseRandomMelee::OnCollide ( sCollision& n_collision ) 
{
	//if ( !CanUse( UPrimary ) && holdState == Holding && bShowTrail )
	//{
		/*
		std::cout << "cc: ";
		if ( n_collision.m_collider_This ) {
			std::cout << n_collision.m_collider_This->GetRigidBody()->GetOwner()->GetTypeName();
		}
		std::cout << " against ";
		if ( n_collision.m_collider_Other ) {
			std::cout << n_collision.m_collider_Other->GetRigidBody()->GetOwner()->GetTypeName();
		}
		else if ( n_collision.m_hit_Other ) {
			std::cout << n_collision.m_hit_Other->GetTypeName();
		}
		std::cout << std::endl;
		*/
	if (( !CanUse( Item::UPrimary ) && holdState == Item::Holding && bShowTrail )&&( n_collision.m_hit_Other && n_collision.m_hit_Other != pOwner ))
	{
		/*std::cout << "cc: ";
		if ( n_collision.m_collider_This ) {
			std::cout << n_collision.m_collider_This->GetRigidBody()->GetOwner()->GetTypeName();
		}
		std::cout << " against ";
		if ( n_collision.m_collider_Other ) {
			std::cout << n_collision.m_collider_Other->GetRigidBody()->GetOwner()->GetTypeName();
		}
		else if ( n_collision.m_hit_Other ) {
			std::cout << n_collision.m_hit_Other->GetTypeName();
		}
		std::cout << std::endl;*/
		Vector4d vrotWeaponArc;
		vrotWeaponArc = pOwner->GetAimingArc();

		// Create weapon ray
		Ray vHitray;
		vHitray.pos = pOwner->GetEyeRay().pos;
		vHitray.dir = pOwner->GetAimRotator() * Vector3d( 1,0,0 );

		CGameBehavior*	hitBehavior	= NULL;
		ftype			multiplier	= 1;
		Item::HitType hittype = Caster::GetHitType( n_collision, &hitBehavior, &multiplier );

		RaycastHit fakeHit;
		fakeHit.hit = true;
		fakeHit.hitPos = n_collision.m_hit.pos;
		fakeHit.hitNormal = n_collision.m_hit.dir;
		fakeHit.pHitBehavior = hitBehavior;
		fakeHit.pHitBody = NULL;//n_collision.m_collider_Other->GetRigidBody();

		// Hit something, deal with it.
		if ( hittype != Item::HIT_NONE && hittype != Item::HIT_UNKNOWN )
		{
			if ( hittype == Item::HIT_ACTOR || hittype == Item::HIT_CHARACTER )
			{
				// Check for a character object
 				if ( hittype == Item::HIT_CHARACTER )
				{
					CCharacter* pHitCharacter = (CCharacter*)hitBehavior;
					if ( pHitCharacter != pOwner )
					{
						if ( find( m_swingHitlist.begin(), m_swingHitlist.end(), hitBehavior ) == m_swingHitlist.end() )
						{
							// Add character to hit list
							m_swingHitlist.push_back( pHitCharacter );
							// Perform damage to the character
							ftype chargeAmount = std::min<ftype>( 1.0f, fChargeTime/weapon_stats.charge_full_time );
							Damage meleeDamage;
							DamageFeedback meleeFeedback;
							// If low stamina on attacker, decrease damage multiplier
							CharacterStats* stats = pOwner->GetCharStats();
							if ( stats ) {
								if ( fStartAttackStamina <= 15.0f ) {
									multiplier *= std::max<ftype>( 0.1f, fStartAttackStamina/15.0f );
									// todo, add imbalance
								}
							}
							meleeDamage.amount	= weapon_stats.damage * Math.Lerp( chargeAmount, weapon_stats.charge_min_damage, weapon_stats.charge_max_damage ) * multiplier;
							meleeDamage.type	= weapon_stats.damagetype;
							meleeDamage.stagger_chance	= weapon_stats.stagger_chance * Math.Lerp( chargeAmount, weapon_stats.charge_min_stagger, weapon_stats.charge_max_stagger );
							if ( bIsDashing ) meleeDamage.stagger_chance = 1.0f;
							meleeDamage.actor	= pOwner;
							meleeDamage.direction = vHitray.dir.normal();
							meleeDamage.source	= transform.position;
							meleeDamage.source.z = n_collision.m_hit.pos.z;
							meleeDamage.applyDirection = Vector3d( (ftype)cos(degtorad(vrotWeaponArc.z)),(ftype)sin(degtorad(vrotWeaponArc.z)),vrotWeaponArc.w*1.2f ).normal();
							if ( pOwner ) pOwner->OnDealDamage( meleeDamage, pHitCharacter );
							pHitCharacter->OnDamaged( meleeDamage, &meleeFeedback );

							// Do material effects
							if ( meleeFeedback.material == NULL ) {
								meleeFeedback.material = PhysMats::Get(PhysMats::MAT_Blood);
							}

							CFXMaterialHit* newHitEffect = new CFXMaterialHit(
								*meleeFeedback.material, fakeHit, CFXMaterialHit::HT_HIT );
							newHitEffect->RemoveReference();

							std::cout << "Sliced a "
								<< hitBehavior->GetTypeName() << " ("
								<< hitBehavior->GetBaseClassName() << ") "
								<< " for " << meleeDamage.amount << " damage."
								<< std::endl;
						}
					}
				}
				// Now, check for other errata
				else
				{
					std::cout << "Sliced a "
						<< hitBehavior->GetTypeName() << " ("
						<< hitBehavior->GetBaseClassName() << ") "
						<< std::endl;
				}
			}
		}
	}
	// 
}

// Five point collision
CActor* CBaseRandomMelee::FivePointCollision ( void )
{
	Vector4d vrotWeaponArc;
	vrotWeaponArc = pOwner->GetAimingArc();
	
	// Now, X is the base angle of the attack
	// Y is the vertical rotation of the attack
	// Z is the general direction of the attack (in degrees)

	// We want 5 different vectors, then
	const int rayCount = 13;
	Vector3d vWeaponRay [rayCount];
	{
		for ( char i = 0; i < rayCount; ++i )
			vWeaponRay[i] = Vector3d( 1,0,0 );

		ftype rotFinalArc = vrotWeaponArc.x + (weapon_stats.reach-1.2f)*8.0f;
		if ( vrotWeaponArc.w > 0.6f )
			rotFinalArc *= 0.6f - vrotWeaponArc.w;

		// Rotate each vector by the needed arc
		//vWeaponRay[0] = Quaternion( Vector3d( 0,0,-rotFinalArc/2 ) ) * vWeaponRay[0];
		//vWeaponRay[1] = Quaternion( Vector3d( 0,0,-rotFinalArc/4 ) ) * vWeaponRay[1];
		//vWeaponRay[3] = Quaternion( Vector3d( 0,0,+rotFinalArc/2 ) ) * vWeaponRay[3];
		//vWeaponRay[4] = Quaternion( Vector3d( 0,0,+rotFinalArc/4 ) ) * vWeaponRay[4];
		for ( int i = 0; i < rayCount/2; ++i ) {
			vWeaponRay[(rayCount/2)-i] = Quaternion( Vector3d( 0,0,-(rotFinalArc/(rayCount-1))*(i+1) ) ) * vWeaponRay[(rayCount/2)-i];
			vWeaponRay[(rayCount/2)+i] = Quaternion( Vector3d( 0,0,+(rotFinalArc/(rayCount-1))*(i+1) ) ) * vWeaponRay[(rayCount/2)+i];
		}

		// Now, rotate all vectors by the general direction
		for ( char i = 0; i < rayCount; ++i ) {
			vWeaponRay[i] = Quaternion( Vector3d( vrotWeaponArc.z, 0,0 ) ) * vWeaponRay[i];
		}
	}

	// Initialize the hitlist (most weapons cannot hit an enemy more than once)
	std::vector<CGameBehavior*> vHitlist = m_swingHitlist;

	ftype armLength = 1.5f;
	if ( bIsDashing ) armLength += 0.7f;

	// Loop through all the traces
	for ( char i = 0; i < rayCount; ++i )
	{
		// Create weapon ray
		Ray vHitray;
		vHitray.pos = pOwner->GetEyeRay().pos;
		vHitray.dir = pOwner->GetAimRotator() * vWeaponRay[i];

		RaycastHit	result;
		BlockTrackInfo	block;	// keep the block for when hitting other things
		CGameBehavior*	hitBehavior;
		ftype			multiplier;
		// Do a raytrace for every point in the 5-point check
		//if ( Raytracer.Raycast( vHitray, weapon_stats.reach+armLength, &result, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31) ) )
		Item::HitType hittype = Caster::Raycast( vHitray, weapon_stats.reach+armLength, &result, &block, &hitBehavior, &multiplier, 0, pOwner );
		if ( hittype != Item::HIT_NONE && hittype != Item::HIT_UNKNOWN )
		{
			if ( hittype == Item::HIT_ACTOR || hittype == Item::HIT_CHARACTER )
			{
				// Check for a character object
 				if ( hittype == Item::HIT_CHARACTER )
				{
					CCharacter* pHitCharacter = (CCharacter*)hitBehavior;
					if ( pHitCharacter != pOwner )
					{
						// Check if the character has been hit already
						if ( find( vHitlist.begin(), vHitlist.end(), pHitCharacter ) == vHitlist.end() )
						{
							// Add character to hit list
							vHitlist.push_back( pHitCharacter );
							// Perform damage to the character
							ftype chargeAmount = std::min<ftype>( 1.0f, fChargeTime/weapon_stats.charge_full_time );
							Damage meleeDamage;
							DamageFeedback meleeFeedback;
							// If low stamina on attacker, decrease damage multiplier
							CharacterStats* stats = pOwner->GetCharStats();
							if ( stats ) {
								if ( fStartAttackStamina <= 15.0f ) {
									multiplier *= std::max<ftype>( 0.1f, fStartAttackStamina/15.0f );
									// todo, add imbalance
								}
							}
							meleeDamage.amount	= weapon_stats.damage * Math.Lerp( chargeAmount, weapon_stats.charge_min_damage, weapon_stats.charge_max_damage ) * multiplier;
							meleeDamage.type	= weapon_stats.damagetype;
							meleeDamage.stagger_chance	= weapon_stats.stagger_chance * Math.Lerp( chargeAmount, weapon_stats.charge_min_stagger, weapon_stats.charge_max_stagger );
							if ( bIsDashing ) meleeDamage.stagger_chance = 1.0f;
							meleeDamage.actor	= pOwner;
							meleeDamage.direction = vHitray.dir.normal();
							meleeDamage.source	= transform.position;
							meleeDamage.source.z = result.hitPos.z;
							meleeDamage.applyDirection = Vector3d( (ftype)cos(degtorad(vrotWeaponArc.z)),(ftype)sin(degtorad(vrotWeaponArc.z)),vrotWeaponArc.w*1.2f ).normal();
							if ( pOwner ) pOwner->OnDealDamage( meleeDamage, pHitCharacter );
							pHitCharacter->OnDamaged( meleeDamage, &meleeFeedback );

							// Do material effects
							if ( meleeFeedback.material == NULL ) {
								meleeFeedback.material = PhysMats::Get(PhysMats::MAT_Blood);
							}
							CFXMaterialHit* newHitEffect = new CFXMaterialHit(
								*meleeFeedback.material, result, CFXMaterialHit::HT_HIT );
							newHitEffect->RemoveReference();

							std::cout << "Sliced a "
								<< hitBehavior->GetTypeName() << " ("
								<< hitBehavior->GetBaseClassName() << ") "
								<< " for " << meleeDamage.amount << " damage."
								<< std::endl;
						}
					}
				}
				// Now, check for other errata
				else
				{
					std::cout << "Sliced a "
						<< hitBehavior->GetTypeName() << " ("
						<< hitBehavior->GetBaseClassName() << ") "
						<< std::endl;
				}
			}
		}
	}	

	if ( vHitlist.empty() )
		return NULL;
	else
		return ((CCharacter*)vHitlist[0]);
}

void CBaseRandomMelee::Attack ( XTransform& )
{
	// Deal damage
	CActor* pHitCharacter;
	//pHitCharacter = FivePointCollision();
	pHitCharacter = NULL;
	// Do view effects
	if ( pHitCharacter )
	{
		// The following is pretty much incorrect.
		if ( pHitCharacter->IsCharacter() ) {
			CCharacter* char_hitCharacter = (CCharacter*)pHitCharacter;
			char_hitCharacter->ApplyLinearMotion( Vector3d( -3.5f,0,0 ), 0.3f );
		}
		ftype chargeAmount = std::min<ftype>( 1.0f, fChargeTime/weapon_stats.charge_full_time );
		if ( pOwner->ActorType() == ACTOR_TYPE_PLAYER ) // Todo: Punch NPC view
		{
			Vector3d vrotWeaponArc;
			vrotWeaponArc = pOwner->GetAimingArc();
			Vector3d punchAmount = Quaternion( Vector3d( vrotWeaponArc.z, 0,0 ) ) * Vector3d(0,0,-weaponItemData.fWeight) * (chargeAmount + 1.0f ) ;
			punchAmount.x = -punchAmount.x;
			((CAfterPlayer*)pOwner)->PunchView( punchAmount );
		}

		//SetCooldown( UPrimary, weapon_stats.recover_time );
		return;
	}
	else
	{
		DoBlockDestruction( Item::UPrimary );
		return;
	}
}

// ================================================
// HUD Element Rendering
// ================================================
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "core/system/Screen.h"
#include "renderer/texture/CBitmapFont.h"
#include "core/settings/CGameSettings.h"

// Render HUD elements
void CBaseRandomMelee::OnDrawHUD ( const WReticleStyle& n_reticleStyle )
{
	if ( !pOwner ) {
		return;
	}
	Vector4d vrotWeaponArc;
	vrotWeaponArc = pOwner->GetAimingArc();

	// Draw default reticle
	GLd_ACCESS;
	n_reticleStyle.matReticle->bindPass(0);
	ftype chargeAmount = std::min<ftype>( 1.0f, fChargeTime/weapon_stats.charge_full_time );
	GLd.P_PushColor( chargeAmount,chargeAmount,chargeAmount,chargeAmount );
	GLd.DrawLineA( 0.5f,0.5f, 0.5f+(ftype)cos(degtorad(vrotWeaponArc.z))*(vrotWeaponArc.x/90.0f)*0.3f,0.5f+(ftype)sin(degtorad(vrotWeaponArc.z))*(vrotWeaponArc.x/90.0f)*0.3f );

	// reticleHalfsize *= CGameSettings::Active()->f_cl_ReticleScale;

	CWeaponItem::OnDrawHUD( n_reticleStyle );
}

// Randomized weapons get a slightly different tooltip that shows their stats.
void CBaseRandomMelee::DrawTooltip ( const Vector2d & drawPos, const WTooltipStyle & style ) 
{
	TooltipDrawBackground( drawPos,style );
	TooltipDrawName( drawPos,style );
	TooltipDrawRarity( drawPos,style );

	GLd_ACCESS;

	{
		ftype yoffset;
		if ( part_info.brand.compare("BASIC") )
			yoffset = 0;
		else if ( part_info.brand.compare("FAT") )
			yoffset = 0.25f;
		else if ( part_info.brand.compare("ELVEN") )
			yoffset = 0.125f;
		else if ( part_info.brand.compare("EASTERN") )
			yoffset = 0.375f;
		else if ( part_info.brand.compare("MERCHANT") )
			yoffset = 0.625f;
		;

		CTexture* bgTex = new CTexture( ".res/textures/Logos/races.png" );
		style.matFont->setTexture( 0, bgTex );
		style.matFont->bindPass(0);
		GLd.P_PushColor( 0.5f,0.5f,0.5f,1.0f );
		GLd.DrawRectangleATex( drawPos.x+0.2f, drawPos.y+0.12f, 0.2f, 0.08f, 0,yoffset+0.0125f,1,0.1f );
		delete bgTex;
	}

	style.matFont->setTexture( 0, style.fontTexture );
	style.matFont->bindPass(0);
	/*GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.08f, "DMG: %.0f", weapon_stats.damage );
	GLd.DrawAutoText( drawPos.x+0.12f, drawPos.y+0.08f, "RCH: %.2f", weapon_stats.reach );
	GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.11f, "TIM: %.2f", weapon_stats.recover_time );
	GLd.DrawAutoText( drawPos.x+0.12f, drawPos.y+0.11f, "HND: %d", weapon_stats.hands );*/
	GLd.DrawAutoText( drawPos.x + 0.02f, drawPos.y + 0.08f, "%d-handed %s", weapon_stats.hands, WeaponItem::GetTypeName(weaponItemData.eItemType) );
	GLd.DrawAutoText( drawPos.x + 0.02f, drawPos.y + 0.10f, "Damage" );
		GLd.DrawAutoText( drawPos.x + 0.20f, drawPos.y + 0.10f, "%.1f", weapon_stats.damage );
	GLd.DrawAutoText( drawPos.x + 0.02f, drawPos.y + 0.12f, "Swing time" );
		GLd.DrawAutoText( drawPos.x + 0.20f, drawPos.y + 0.12f, "%.1f sec", weapon_stats.recover_time );
	GLd.DrawAutoText( drawPos.x + 0.02f, drawPos.y + 0.14f, "Reach" );
		GLd.DrawAutoText( drawPos.x + 0.20f, drawPos.y + 0.14f, "%.1f ft", weapon_stats.reach );
	TooltipDrawDescription( drawPos+Vector2d(0,0.09f),style );
	TooltipDrawPicture( drawPos,style );
}

// -----------------------------------------------
// Block Destruction Code
// -----------------------------------------------

#include "after/types/terrain/BlockType.h"
#include "after/entities/props/CTerrainProp.h"

void CBaseRandomMelee::InitializeBlockDestruction ( void )
{
	// Initialize Default Values
	iHitMutliplier	= 1;
	fCooldownOnHit	= 0.4f;
	fCooldownOnMiss = 0.7f;
	//fMaxToolRange	= 7.0f;

	// Set Default Block Strengths
	memcpy( cBlockHardness, Terrain::cDefaultBlockHardness, 1024 );
	cBlockHardness[Terrain::EB_DIRT]		= 6;
	cBlockHardness[Terrain::EB_GRASS]		= 5;
	cBlockHardness[Terrain::EB_SAND]		= 8;
	cBlockHardness[Terrain::EB_CLAY]		= 6;
	cBlockHardness[Terrain::EB_WOOD]		= 4;
	cBlockHardness[Terrain::EB_WIRE]		= 1;
}
bool CBaseRandomMelee::DoBlockDestruction ( int x )
{
	if ( !bHardnessSet ) {
		SetBlockHardness();
		bHardnessSet = true;
	}

	//const ftype armLength = 1.3f;
	ftype armLength = ( 4.4f - weapon_stats.reach )*0.65f;

	// Do a raycast
	Ray viewRay = pOwner->GetEyeRay();
	RaycastHit	result;
	BlockTrackInfo	block;
	CGameBehavior*	hitBehavior;
	block.block.block = Terrain::EB_NONE;

	//if ( Raytracer.Raycast( viewRay, weapon_stats.reach+3.0f+armLength, &result, &block, 1|2|4 ) )
	Item::HitType hittype = Caster::Raycast( viewRay, weapon_stats.reach+3.0f+armLength, &result, &block, &hitBehavior, NULL, 0, pOwner );
	if ( hittype != Item::HIT_NONE && hittype != Item::HIT_UNKNOWN )
	{
		// CVoxelTerrain::terrainList[0] is always guarenteed to be the current active terrain, even if there are multiple terrains existing.
		//if (( !CVoxelTerrain::terrainList.empty() )&&( result.pHitBehavior == CVoxelTerrain::terrainList[0] ))
		if ( hittype == Item::HIT_TERRAIN )
		{
			if (( block.block.block == Terrain::EB_NONE )||( block.block.block == Terrain::EB_WATER )) {
				// Set cooldown if miss
				SetCooldown( x, fCooldownOnMiss * (0.7f/weapon_stats.recover_time) );
				return false;
			}
			bool found = false;
			// First check the queue for the specific terrain hit
			for ( std::list<HitPartInfo>::iterator it = hitList.begin(); it != hitList.end(); it++ )
			{
				//hitList.push_front();
				// If it matches, move it to the front
				//if ( it->blockInfo == block )
				/*if (
					( it->blockInfo.b16index == block.b16index )&&( it->blockInfo.b8index == block.b8index )&&( it->blockInfo.b1index == block.b1index )
					&&( it->blockInfo.pBoob == block.pBoob )
					)*/
				if (
					( it->blockInfo.pBlock == block.pBlock ) ||
					( it->blockInfo.pos_x == block.pos_x && it->blockInfo.pos_y == block.pos_y && it->blockInfo.pos_z == block.pos_z )
					)
				{
					found = true;
					HitPartInfo temp = (*it);
					hitList.erase( it );
					hitList.push_front( temp );
					break;
				}
			}
			// If it's not found, push a new hit to the front
			if ( !found )
			{
				HitPartInfo newHit;
				newHit.treePart = NULL;
				newHit.hitCount = 0;
				newHit.pTree = NULL;
				newHit.blockInfo = block;
				newHit.hasItemDrop = false;
				hitList.push_front( newHit );
			}
			// The current hit should now be at the front. Work on that.
			hitList.front().hitCount += 1;

			// Do material effects
			CFXMaterialHit* newHitEffect = new CFXMaterialHit(
				Terrain::MaterialOf( hitList.front().blockInfo.block.block ),
				result, CFXMaterialHit::HT_HIT );
			newHitEffect->RemoveReference();

			// If the current hit's hit count is above the threshold, remove the block and the hit
			bool hitThreshold = false;
			if ( hitList.front().hitCount / iHitMutliplier >= GetHitCount( cBlockHardness[hitList.front().blockInfo.block.block] ) ) {
				hitThreshold = true;
			}
			if ( !hitList.front().hasItemDrop )
			{
				// If it's the second to last hit, then do a possible item drop.
				if ( ItemGenerator.ChanceDiggingDrops() ) {
					hitList.front().hasItemDrop = true;
				}
				if ( hitList.front().hasItemDrop )
				{
					hitList.front().hitCount -= 1;
					hitThreshold = false;
					// Play OUCH animation
				}
			}
			if ( hitThreshold )
			{
				// Destructive Power
				if ( cBlockHardness[hitList.front().blockInfo.block.block] >= 0 )
				{
					// Create item drop if it has it
					if ( hitList.front().hasItemDrop ) {
						ItemGenerator.DropDigging( result.hitPos, hitList.front().blockInfo.block.block );
						// Play OUCH animation
					}
					// Itemize block
					//CVoxelTerrain::terrainList[0]->DestroyBlock( hitList.front().blockInfo );
					CTerrainAccessor accessor;
					accessor.DestroyBlock( hitList.front().blockInfo );
					// Remove it from the list
					hitList.pop_front();

					// Give experience for mining with hands (small amount)
					if ( pOwner->layer & Layers::Character ) {
						((CCharacter*)pOwner)->OnGainExperience( Experience(2,DscMining) );
					}
				}
				else // Constructive Power
				{
					// Do special things...such as sand to sandstone.
					// Coal to diamond or a different gem depending on nearby dyes.
					// Rocks to gravel to make some slingshot ammo.
					//CVoxelTerrain::terrainList[0]->CompressBlock( hitList.front().blockInfo );
					CTerrainAccessor accessor;
					accessor.CompressBlock( hitList.front().blockInfo );
				}
			}
			else
			{
				// If it's a top hit, then destroy some grass.
				if ( result.hitNormal.z > 0.5f ) {
					//hitList.front().blockInfo.pBoob->pGrass->BreakGrass( hitList.front().blockInfo.pBlock );
					CTerrainAccessor accessor;
					accessor.DestroyGrass( hitList.front().blockInfo );
				}
			}
		}
		//else if ( ((CRigidBody*)result.pHitBehavior)->GetOwner()->GetBaseClassName() == "CFoliage_TreeBase" )
		else if ( hittype == Item::HIT_TREE )
		{
			bool found = false;
			
			//CTreeBase* pHitTree = (CTreeBase*)(((CRigidBody*)result.pHitBehavior)->GetOwner());
			CTreeBase* pHitTree = (CTreeBase*)hitBehavior;
			TreePart* pTreePartHit = pHitTree->GetPartClosestTo( result.hitPos );

			std::cout << "Hit a tree." << " part: " << pTreePartHit << " tree: " << pHitTree << std::endl;

			//DebugD::DrawLine( pTreePartHit->shape.pos+pHitTree->transform.position, result.hitPos );

			// Do material effects
			CFXMaterialHit* newHitEffect = new CFXMaterialHit(
				*PhysMats::Get(PhysMats::MAT_Wood),
				result, CFXMaterialHit::HT_HIT );
			newHitEffect->RemoveReference();

			// First check the queue for the specific tree part hit
			for ( std::list<HitPartInfo>::iterator it = hitList.begin(); it != hitList.end(); it++ )
			{
				// If it matches, move it to the front
				if ( it->treePart == pTreePartHit )
				{
					found = true;
					HitPartInfo temp = (*it);
					hitList.erase( it );
					hitList.push_front( temp );
					break;
				}
			}
			// If it's not found, push a new hit to the front
			if ( !found )
			{
				HitPartInfo newHit;
				newHit.treePart = pTreePartHit;
				newHit.hitCount = 0;
				newHit.pTree = pHitTree;
				newHit.blockInfo = block;
				hitList.push_front( newHit );
			}
			// The current hit should now be at the front. Work on that.
			hitList.front().hitCount += 1;

			// If the current hit's hit count is above the threshold, remove the block and the hit
			bool hitThreshold = false;
			if ( hitList.front().hitCount / iHitMutliplier >= GetHitCount( cBlockHardness[Terrain::EB_WOOD] ) )
				hitThreshold = true;
			if ( hitThreshold )
			{
				//CVoxelTerrain::terrainList[0]->ItemizeBlock( hitList.front().blockInfo );
				hitList.front().pTree->BreakPart( hitList.front().treePart, true );
				hitList.pop_front();
			}
		}
		//else if ( ((CRigidBody*)result.pHitBehavior)->GetOwner()->GetBaseClassName() == "CGameObject_TerrainProp" )
		else if ( hittype == Item::HIT_COMPONENT )
		{
			// Get component pointer
			CTerrainProp* pHitComponent = (CTerrainProp*)hitBehavior;

			// Output debug
			std::cout << "Punched a component: " << pHitComponent << ", " << pHitComponent->GetTypeName() << std::endl;

			// Call the component onPunch
			pHitComponent->OnPunched( result );
		}
		else
		{
			std::cout << "Punched a "
				<< hitBehavior->GetTypeName() << " ("
				<< hitBehavior->GetBaseClassName() << ") "
				<< std::endl;
		}
		

		// Pop back if too large of a list
		if ( hitList.size() > 4 )
			hitList.pop_back();

		// Set cooldown if hit
		SetCooldown( x, fCooldownOnHit * (0.7f/weapon_stats.recover_time) );

		return true;
	}
	else
	{
		// Set cooldown if miss
		//SetCooldown( x, fCooldownOnMiss * (0.7f/weapon_stats.recover_time) );

		return false;
	}	
}

// Returns amount of hits needed to destroy something with the given hardness
short CBaseRandomMelee::GetHitCount ( char hardness )
{
	// Algebraic: "=floor( A*e^( B*sqrt(x) + C ) )"
	// LibreOffice Calc: "=ROUNDDOWN(F3*EXP( H3 + G3*SQRT(A3) ))"
	if ( hardness >= 0 )
	{
		return short( 0.00003f * exp( 10.05f + 0.95f*sqrt( float(hardness) ) ) );
	}
	else
	{
		return short( 0.00003f * exp( 10.05f + 0.95f*sqrt( float(-hardness) ) ) );
	}
}