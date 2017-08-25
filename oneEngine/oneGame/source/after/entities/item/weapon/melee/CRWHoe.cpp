
#include "CRWHoe.h"
#include "renderer/logic/model/CModel.h"
#include "after/entities/character/CAfterPlayer.h"

CRWHoe::CRWHoe ( void )
	: CBaseRandomMelee( tMeleeWeaponProperties(), ItemData() )
{
	holdType = Item::TwoHandedAxe;
}

CRWHoe::~CRWHoe ( void )
{
	;
}

void CRWHoe::Generate ( void )
{
	CBaseRandomMelee::Generate();

	
	string pickModel = "models/items/pickaxe_su_shitty.FBX";
	pModel = new CModel( pickModel );


	base_weapon_stats.damagetype	= DamageType::Slash;
	base_weapon_stats.reach			= 1.6f;
	base_weapon_stats.damage		= 12.0f;
	base_weapon_stats.hands			= 2;
	base_weapon_stats.recover_time	= 1.4f; 
}


// ATTACK WITH HOE.
#include "engine-common/entities/CPlayer.h"
void CRWHoe::Attack ( XTransform& ) 
{
	CActor* pHitCharacter;
	pHitCharacter = FivePointCollision();
	if ( pHitCharacter )
	{
		ftype chargeAmount = std::min<ftype>( 1.0f, GetChargeTime()/weapon_stats.charge_full_time );
		//CPlayer* activePlayer = (CPlayer*) CPlayer::GetActivePlayer();
		if ( pOwner && pOwner->ActorType() == ACTOR_TYPE_PLAYER )
		{
			Vector3d vrotWeaponArc;
			vrotWeaponArc = pOwner->GetAimingArc();
			Vector3d punchAmount = Quaternion( Vector3d( vrotWeaponArc.z, 0,0 ) ) * Vector3d(0,0,-weaponItemData.fWeight) * (chargeAmount + 1.0f ) ;
			punchAmount.x = -punchAmount.x;
			((CAfterPlayer*)pOwner)->PunchView( punchAmount ); // Todo: Make this an NPC function
		}

		//SetCooldown( UPrimary, weapon_stats.recover_time );
		return;
	}
	else
	{
		//DoBlockDestruction( UPrimary ); // NO. DON'T DO BLOCK DESTRUCTION. DO GODDAMN FARMING.
		DoBlockFarming();
		return;
	}
}

// -----------------------------------------------
// Farming Code
// -----------------------------------------------
//#include "CTerrainAccessor.h"
#include "engine-common/entities/effects/CFXMaterialHit.h"
#include "after/types/terrain/BlockType.h"
#include "engine/physics/raycast/Raycaster.h"
#include "after/physics/Caster.h"


void CRWHoe::DoBlockFarming ( void )
{
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
				SetCooldown( Item::UPrimary, fCooldownOnMiss * (0.7f/weapon_stats.recover_time) );
				return;
			}

			// Do material effects
			CFXMaterialHit* newHitEffect = new CFXMaterialHit(
				Terrain::MaterialOf( block.block.block ),
				result, CFXMaterialHit::HT_HIT );
			newHitEffect->RemoveReference();

			//if ( hitThreshold )
			CTerrainAccessor accessor;
			// If it's a top hit and nothing is there....
			if ( (result.hitNormal.z > 0.5f) && !accessor.BlockHasComponent( block ) && !accessor.BlockHasFoliage( block ) )
			{
				if ( accessor.BlockHasGrass( block ) )
				{
					// Destroy some grass.
					accessor.DestroyGrass( block );
				}
				else
				{
					// Create item drop if it has it
					/*{
						ItemGenerator.DropTilling( result.hitPos, hitList.front().blockInfo.block.block );
						// Play OUCH animation
					}*/

					// Itemize block
					CTerrainAccessor accessor;
					//accessor.DestroyBlock( block );
					accessor.TillBlock( block );

					// Give experience for mining with hands (small amount)
					if ( pOwner->layer & Layers::Character ) {
						((CCharacter*)pOwner)->OnGainExperience( Experience(1,DscFarming) );
					}
				}
			}
		}
		else
		{
			// Play OUCH animation
		}
	}
}