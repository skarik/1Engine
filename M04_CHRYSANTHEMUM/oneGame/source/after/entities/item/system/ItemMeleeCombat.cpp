

#include "ItemMeleeCombat.h"
#include "after/entities/character/CCharacter.h"
#include "after/types/character/Dialogue.h"

#include "after/interfaces/CCharacterMotion.h"

#include "after/types/terrain/BlockType.h"
#include "core/debug/CDebugConsole.h"

ItemMeleeCombat::ItemMeleeCombat ( void )
	: CWeaponItem( ItemData() )
{
	chargeTime_Heavy = 0.4f;
	chargeTime = 0;

	// Set initial state as idle
	attackstate = ATTACKSTATE_IDLE;

	// Set holdtype as empty handed
	// This allows melee animation requests to go through
	holdType = Item::EmptyHanded;

}

ItemMeleeCombat::~ItemMeleeCombat ( void )
{
	;
}


void ItemMeleeCombat::CreatePhysics ( void )
{
	// empty
}


// Use function
bool ItemMeleeCombat::Use ( int x )
{
	if ( x == Item::UPrimary || x == Item::USecondary || x == Item::UPrimarySprint || x == Item::USecondarySprint )
	{
		if ( !CanUse( Item::UPrimary ) )
			return true;
		chargeTime += Time::deltaTime;
		return true;
	}

	return false;
}
// Activating the item has just started. Weapons take the firing type as the argument.
void ItemMeleeCombat::StartUse ( int x )
{
	if ( x == Item::UPrimary || x == Item::USecondary || x == Item::UPrimarySprint || x == Item::USecondarySprint )
	{
		if ( !CanUse( Item::UPrimary ) )
			return;
		// Reset charge time
		chargeTime = 0.0f;
		charged = true;
	}
}
// Activating the item has ended. Weapons take the firing type as the argument.
void ItemMeleeCombat::EndUse ( int x )
{
	// TODO: Check for crouch, jump, sliding
	if ( charged && (x == Item::UPrimary || x == Item::USecondary || x == Item::UPrimarySprint || x == Item::USecondarySprint) )
	{
		std::cout << "Quick attack" << std::endl;

		// Depending on the click, output the attack type
		if ( chargeTime < chargeTime_Heavy )
		{
			// Set movement mode first
			CCharacterMotion* t_motion = ((CCharacter*)pOwner)->GetMotionState();
			if ( t_motion )
			{
				// Depending on the motion type, perform different attack types (most motion types do NOT have attack associated with them)
				attackstate = ATTACKSTATE_IDLE; // Start out by "zeroing out" the attack type
				switch ( t_motion->GetMovementMode() )
				{
					// Ground, Sprint, and Crouch attack
				case NPC::MOVEMENT_ONGROUND:
					if ( x != Item::UPrimarySprint && x != Item::USecondarySprint ) {
						if ( !t_motion->bIsCrouching && !t_motion->bIsProne ) {
							attackstate = ATTACKSTATE_ATKGROUND;
						}
						else if ( t_motion->bIsCrouching ) {
							attackstate = ATTACKSTATE_ATKDUCK;
						}
					}
					else {
						attackstate = ATTACKSTATE_ATKSPRINT;
					}
					break;
					// Falling attack
				case NPC::MOVEMENT_FALLING:
					attackstate = ATTACKSTATE_ATKAIR;
					break;
					// Slide attack
				case NPC::MOVEMENT_PARKOUR_SLIDE:
					attackstate = ATTACKSTATE_ATKSLIDE;
					break;
				}

				// Based on the calculated attack state we want, perform an animation + cooldowns + state set. Rest should be taken care of by animation system.
				switch ( attackstate )
				{
					// Ground attack
				case ATTACKSTATE_ATKGROUND:
					t_motion->SetMovementModeQueued( NPC::MOVEMENT_ATK_GROUND );
					SetCooldown( Item::UPrimary, 1.0f );	

					if ( x == Item::UPrimary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_R, 0,0,0 );
					}
					else if ( x == Item::USecondary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_L, 0,0,0 );
					}
					break;

					// Sprint attack
				case ATTACKSTATE_ATKSPRINT:
					t_motion->SetMovementModeQueued( NPC::MOVEMENT_ATK_GROUND );
					SetCooldown( Item::UPrimary, 1.0f );	

					if ( x == Item::UPrimary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_R, 0,0,0 );
					}
					else if ( x == Item::USecondary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_L, 0,0,0 );
					}
					break;

					// Duck attack
				case ATTACKSTATE_ATKDUCK:
					t_motion->SetMovementModeQueued( NPC::MOVEMENT_ATK_GROUND );
					SetCooldown( Item::UPrimary, 1.0f );	

					if ( x == Item::UPrimary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_R, 0,0,0 );
					}
					else if ( x == Item::USecondary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_L, 0,0,0 );
					}
					break;

					// Slide attack
				case ATTACKSTATE_ATKSLIDE:
					t_motion->SetMovementModeQueued( NPC::MOVEMENT_ATK_SLIDE );
					SetCooldown( Item::UPrimary, 1.0f );	

					if ( x == Item::UPrimary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_R, 0,0,0 );
					}
					else if ( x == Item::USecondary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_L, 0,0,0 );
					}
					break;

					// Fall attack
				case ATTACKSTATE_ATKAIR:
					t_motion->SetMovementModeQueued( NPC::MOVEMENT_ATK_AIR );
					SetCooldown( Item::UPrimary, 1.0f );	

					if ( x == Item::UPrimary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_R, 0,0,0 );
					}
					else if ( x == Item::USecondary ) {
						((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hPunch_L, 0,0,0 );
					}
					break;
				}
				// End motion checks
			} //
			else
			{
				Debug::Console->PrintError( "MeleeCombat does not function without a motion system." );
			}
			// Check if actually attacked
			if ( attackstate != ATTACKSTATE_IDLE )
			{
				// Play character yell
				((CCharacter*)pOwner)->DoSpeech( NPC::SpeechType_Kiai );
			}
		}
		else 
		{
			std::cout << "Heavy attack" << std::endl;

			((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hKick_L, 0,0,0 );
			// Check if actually attacked
			if ( attackstate != ATTACKSTATE_IDLE )
			{
				// Play character yell
				((CCharacter*)pOwner)->DoSpeech( NPC::SpeechType_Kiai );
			}
		}

		// Reset state
		chargeTime = 0.0f;
		charged = false;
	}
}

// Reset the item state on equipping
void ItemMeleeCombat::OnEquip ( CActor* interactingActor )
{
	// Set initial state as idle
	attackstate = ATTACKSTATE_IDLE;

	std::cout << "equipping hands" << std::endl;

	// put stopping other melee idle animations here
}

// Reset the view state and movement state when not being equipped
void ItemMeleeCombat::OnUnequip ( CActor* interactingActor )
{
	((CCharacter*)pOwner)->SetViewAngleOffset();
	((CCharacter*)pOwner)->SetMovementSpeedScale( 1.0f );
	CCharacterMotion* t_motion = ((CCharacter*)pOwner)->GetMotionState();
	if ( t_motion ) {
		t_motion->m_canJump = true;
	}

	charged = false;
	chargeTime = 0.0f;

	std::cout << "unequipping hands" << std::endl;
}

// Update
void ItemMeleeCombat::Update ( void )
{
	((CCharacter*)pOwner)->SetViewAngleOffset( std::min<Real>( std::max<Real>( 0.0f, (chargeTime - chargeTime_Heavy) * 5.0f ), 5.0f ) );

	CCharacterMotion* t_motion = ((CCharacter*)pOwner)->GetMotionState();
	if (t_motion) // Check for motion, as can only do proper melee combat w/ motion enabled
	{
		// Grab proper movetype and attack cancelling checks
		NPC::eMovementEnumeration t_movetypeAttack = NPC::MOVEMENT_ATK_GROUND;
		NPC::eMovementEnumeration t_movetypeRelax = NPC::MOVEMENT_ONGROUND;
		if ( attackstate == ATTACKSTATE_ATKGROUND )
		{
			t_movetypeAttack = NPC::MOVEMENT_ATK_GROUND;
			t_movetypeRelax = NPC::MOVEMENT_ONGROUND;
		}
		else if ( attackstate == ATTACKSTATE_ATKAIR )
		{
			t_movetypeAttack = NPC::MOVEMENT_ATK_AIR;
			t_movetypeRelax = NPC::MOVEMENT_FALLING;
		}
		else if ( attackstate == ATTACKSTATE_ATKSLIDE )
		{
			t_movetypeAttack = NPC::MOVEMENT_ATK_SLIDE;
			t_movetypeRelax = NPC::MOVEMENT_PARKOUR_SLIDE;
		}
		// Perform UPrimary check
		if ( t_motion->GetMovementMode() == t_movetypeAttack )
		{
			if ( CanUse(Item::UPrimary) )
			{
				t_motion->SetMovementModeQueued( t_movetypeRelax );
			}
		}
		else // Somehow am not on ground anymore
		{
			if ( !CanUse(Item::UPrimary) )
			{
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::h2hInterrupt, 0,0,0 );
			}
		}

		// Setup motion
		if (!CanUse(Item::UPrimary) )
		{
			t_motion->m_canJump = false;
			t_motion->m_canCombatSlide = false;
			((CCharacter*)pOwner)->SetMovementSpeedScale( 0.0f );
		}
		else
		{
			t_motion->m_canJump = true;
			t_motion->m_canCombatSlide = true;
			((CCharacter*)pOwner)->SetMovementSpeedScale( 1.0f );
		}
	}
}

#include "engine/physics/raycast/Raycaster.h"
#include "renderer/debug/CDebugDrawer.h"
#include "engine-common/entities/effects/CFXMaterialHit.h"

// Attack function
void ItemMeleeCombat::Attack ( XTransform& n_attackTransform )
{
	std::cout << "ATK" << std::endl;
	// Perform hitbox? Raytrace?

	// Hitbox definitely.
	//Raytracer.Raycast(
	//Raytracer.Raycast( 

	//hkp
	hkpShape* t_shape = Physics::CreateBoxShape( Vector3d(1,1,1), Vector3d::zero );
	hkTransform t_transform;
	t_transform.setIdentity();
	hkpCollidable* t_collidable = new hkpCollidable( t_shape, &t_transform );

	Vector3d t_targetPosition = n_attackTransform.position;//((CCharacter*)pOwner)->GetHoldTransform(0).position;
	t_transform.setTranslation( hkVector4( t_targetPosition.x, t_targetPosition.y, t_targetPosition.z ) );

	hkpAllCdPointCollector t_collector;
	t_collector.reset();
	hkpCollisionInput input = *Physics::GetCollisionCollector();
	Physics::GetClosestPoints( t_collidable, input, t_collector );

	// show them hits
	// iterate over each individual hit
	for (int j = 0; j < t_collector.getHits().getSize(); j++ )
	{
		//displayRootCdPoint( world, collector[i].getHits()[j] );
		hkVector4 hitPosition = t_collector.getHits()[j].m_contact.getPosition();
		hkVector4 hitNormal = t_collector.getHits()[j].m_contact.getNormal();
		Vector3d t_hitPos = Vector3d(
			hitPosition.getComponent<0>(),
			hitPosition.getComponent<1>(),
			hitPosition.getComponent<2>()
			);
		Vector3d t_hitNorm = Vector3d(
			hitNormal.getComponent<0>(),
			hitNormal.getComponent<1>(),
			hitNormal.getComponent<2>()
			);
		Debug::Drawer->DrawLine( t_hitPos, t_hitPos+t_hitNorm*2.0f, Color(1,0,0) );

		RaycastHit rhLastHit;
		rhLastHit.hitPos = t_hitPos;
		rhLastHit.hitNormal = t_hitNorm;
		// Do material effects
		CFXMaterialHit* newHitEffect = new CFXMaterialHit(
			//Terrain::MaterialOf( Raycaster.HitBlock().block.block ),
			Terrain::MaterialOf( TerrainAccess.GetBlockAtPosition( rhLastHit ).block ),
			rhLastHit, CFXMaterialHit::HT_STEP );
		newHitEffect->RemoveReference();
	}
}
