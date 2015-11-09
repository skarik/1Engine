// Includes
#include "CAfterPlayer.h"
#include "CMccCharacterModel.h"

#include "core/math/random/Random.h"

#include "engine/audio/CAudioInterface.h"
#include "engine/physics/raycast/Raycaster.h"

#include "engine-common/entities/effects/CFXMaterialHit.h"
#include "engine-common/entities/CParticleSystem.h"

#include "after/physics/water/Water.h"
#include "after/terrain/Zones.h"
#include "after/entities/item/weapon/CBaseRandomMelee.h"
#include "after/interfaces/CSpeechTrie.h"
#include "after/types/terrain/BlockType.h"
#include "after/terrain/edit/CTerrainAccessor.h"

// Speech
void CAfterPlayer::DoSpeech ( const NPC::eGeneralSpeechType& speechType )
{
	animator.DoSpeech( speechType );
}

// SpeakDialogue makes this character speak dialogue. It takes either a raw path or a sound file
void CAfterPlayer::SpeakDialogue ( const string& soundFile )
{
	animator.SpeakDialogue( soundFile );
}

// == Animation Movement ==
void CAfterPlayer::SetMoveAnimation ( const NPC::eMoveAnimType newMoveType )
{
	animator.SetMoveAnimation( newMoveType );
}

// == Animation Events ==
void CAfterPlayer::OnAnimationEvent ( const Animation::eAnimSystemEvent eventType, const Animation::tag_t tag )
{
	static CParticleSystem* t_slideSystem = NULL;
	switch ( eventType )
	{
	case Animation::Event_Attack:
		{
			CWeaponItem* currentEquipped = pMyInventory->GetCurrentEquipped();
			if ( currentEquipped ) {
				XTransform t_targetTransform ( currentEquipped->transform.position, currentEquipped->transform.rotation );
				if ( tag != Animation::EventTag_NoTag && tag != Animation::EventTag_Default ) {
					model->GetBoneTransform( tag, t_targetTransform );
				}
				currentEquipped->Attack( t_targetTransform );	// Call attack event
			}
		}
		break;
	case Animation::Event_ClangCheck:
		{
			// Run CLANG CHECK on the weapon
			CWeaponItem* currentEquipped = pMyInventory->GetCurrentEquipped();
			if ( currentEquipped ) {
				if ( currentEquipped->GetBaseClassName() == "RandomBaseMelee" ) {
					((CBaseRandomMelee*)(currentEquipped))->ClangCheck();
				}
			}
		}
		break;
	case Animation::Event_Footstep:
	case Animation::Event_Footstep_Left:
	case Animation::Event_Footstep_Right:
		{
			//Audio.playSound( "Char.Footstep" )->position = transform.position;
			Ray castRay;
			castRay.pos = transform.position + Vector3d( 0,0,1.8f );
			if ( eventType == Animation::Event_Footstep_Left ) {
				XTransform footTransform;
				model->GetLFootTransform(0,footTransform);
				castRay.pos.x = footTransform.position.x;
				castRay.pos.y = footTransform.position.y;
			}
			else if ( eventType == Animation::Event_Footstep_Right ) {
				XTransform footTransform;
				model->GetRFootTransform(0,footTransform);
				castRay.pos.x = footTransform.position.x;
				castRay.pos.y = footTransform.position.y;
			}

			castRay.dir = Vector3d( 0,0,-1 );
			RaycastHit rhLastHit;
			if ( Raycaster.Raycast( castRay, 2.4f, &rhLastHit, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), this ) )
			{
				// Do material effects
				CFXMaterialHit* newHitEffect = new CFXMaterialHit(
					//Terrain::MaterialOf( Raycaster.HitBlock().block.block ),
					Terrain::MaterialOf( TerrainAccess.GetBlockAtPosition(rhLastHit).block ),
					rhLastHit, CFXMaterialHit::HT_STEP );
				newHitEffect->RemoveReference();
			}
		}
		break;
	case Animation::Event_Slide:
		{
			if ( !t_slideSystem ) {
				t_slideSystem = new CParticleSystem( "particlesystems/slidepoof.pcf" );
			}

			// Cast downward from the feet
			Ray castRay;
			castRay.pos = transform.position + Vector3d( 0,0,1.8f );
			{
				XTransform footTransform;
				model->GetLFootTransform(0,footTransform);
				castRay.pos.x = footTransform.position.x;
				castRay.pos.y = footTransform.position.y;
			}
			{
				XTransform footTransform;
				model->GetRFootTransform(0,footTransform);
				castRay.pos.x += footTransform.position.x;
				castRay.pos.y += footTransform.position.y;
			}
			castRay.pos.x /= 2;
			castRay.pos.y /= 2;

			castRay.dir = Vector3d( 0,0,-1 );
			RaycastHit rhLastHit;
			if ( Raycaster.Raycast( castRay, 2.4f, &rhLastHit, Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), this ) )
			{
				t_slideSystem->enabled = true;
				t_slideSystem->transform.position = rhLastHit.hitPos;
			}
			else
			{
				t_slideSystem->enabled = false;
				t_slideSystem->transform.position = transform.position;
			}
		}
		break;
	case Animation::Event_SlideEnd:
		{
			if ( t_slideSystem ) 
			{
				t_slideSystem->enabled = false;
			}
		}
		break;
	case Animation::Event_INVALID:
		{
			std::cout << "The fuck" << std::endl;
		}
		break;
	}
}

// ===Base Animation Interface===
//void	CAfterPlayer::PlayItemAnimation ( const string& sActionName, const float fArg )
void	CAfterPlayer::PlayItemAnimation ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand, const float fArg, const float fAnimSpeed, const float fAttackSkip )
{
	animator.PlayItemAnimation( nActionName, nSubset, nHand, fArg, fAnimSpeed, fAttackSkip );
}
Real	CAfterPlayer::GetItemAnimationLength ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand )
{
	return animator.GetItemAnimationLength( nActionName, nSubset, nHand );
}
void	CAfterPlayer::PlayAnimation ( const string& sActionName )
{
	std::cout << "NO ANIMATION FOR " << sActionName << " IS AVAILABLE" << std::endl;
}


// == Character State Getters ==
// Is this character performing a melee attack? The input to the function is the hand to check.
// If the hand is being used with a melee attack, then return true.
bool	CAfterPlayer::IsAttackingMelee ( const short hand )
{
	int frame = model->GetMeleeAttackFrame( hand );

	return frame>=0;
}
// Get the frame of the melee attack that the character is attacking. This is used for determining attack cancelling during melee attacks.
// Attack cancelling is used for both combos and for sword clashing.
int		CAfterPlayer::GetAttackingFrame ( const short hand )
{
	return model->GetMeleeAttackFrame( hand );
}