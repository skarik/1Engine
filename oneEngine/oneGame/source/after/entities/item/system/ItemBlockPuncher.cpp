
// joins game
// how do i shot web
// i dunno spidey lol

#include "ItemBlockPuncher.h"
#include "after/terrain/edit/CTerrainAccessor.h"
#include "after/entities/character/CAfterPlayer.h"
#include "engine-common/entities/effects/CFXMaterialHit.h"
#include "after/entities/item/weapon/CWeaponItemGenerator.h"
#include "after/types/terrain/BlockType.h"
#include "after/physics/Caster.h"

#include "after/entities/foliage/CTreeBase.h"
#include "after/entities/props/CTerrainProp.h"


ItemBlockPuncher::ItemBlockPuncher ( void )
	: CWeaponItem( ItemData() )
{
	// Initialize Default Values
	iHitMutliplier	= 1;
	fCooldownOnHit	= 0.4f;
	fCooldownOnMiss = 0.7f;
	fMaxToolRange	= 7.0f;

	// Set Default Block Strengths
	memcpy( cBlockHardness, Terrain::cDefaultBlockHardness, 1024 );
	cBlockHardness[Terrain::EB_DIRT]		= 4;
	cBlockHardness[Terrain::EB_GRASS]		= 5;
	cBlockHardness[Terrain::EB_SAND]		= 4;
	cBlockHardness[Terrain::EB_CLAY]		= 6;
	cBlockHardness[Terrain::EB_WOOD]		= 6;
	cBlockHardness[Terrain::EB_WIRE]		= 2;


	holdType = Item::EmptyHanded;
}

ItemBlockPuncher::~ItemBlockPuncher ( void )
{
	
}

void ItemBlockPuncher::Update ( void )
{
	
}

void ItemBlockPuncher::CreatePhysics ( void )
{
	// empty
}

bool ItemBlockPuncher::Use( int x )
{
	if ( !CanUse( x ) )
		return true;
	if ( x == Item::UPrimary )
	{
		// Don't even attempt if there's no stamina to try with
		if ( pOwner && ( pOwner->GetTypeName() == "CPlayer" ) )
		{
			CAfterPlayer*	pPlayer = (CAfterPlayer*)pOwner;
			if ( pPlayer->GetPlayerStats()->stats->fStamina < 8.0f )
				return true;
		}
		// If there's no owner, should even be here, but sometimes happens sometimes
		if ( !pOwner ) {
			Debug::Console->PrintError( "Itempuncher used with no owner!" );
			return true;
		}

		// Do a raycast
		Ray viewRay = pOwner->GetEyeRay();
		RaycastHit	result;
		BlockTrackInfo	block;
		CGameBehavior*	hitBehavior;
		block.block.block = Terrain::EB_NONE;

		//if ( Raytracer.Raycast( viewRay, fMaxToolRange, &result, &block, 1|2|4 ) )
		Item::HitType hittype = Caster::Raycast( viewRay, fMaxToolRange, &result, &block, &hitBehavior, NULL, 0, pOwner );
		if ( hittype != Item::HIT_NONE && hittype != Item::HIT_UNKNOWN )
		{
			// Based on the result and block hit, check if is in list/queue
			// CVoxelTerrain::terrainList[0] is always guarenteed to be the current active terrain, even if there are multiple terrains existing.
			if ( hittype == Item::HIT_TERRAIN )
			{
				if (( block.block.block == Terrain::EB_NONE )||( block.block.block == Terrain::EB_WATER )) {
					// Set cooldown if miss
					SetCooldown( x, fCooldownOnMiss );
					return false;
				}

				// Play PAUNCH hit animation
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::PunchNeutral, iAnimationSubset, 0, 0 );
			}

			// Set cooldown if hit
			SetCooldown( x, fCooldownOnHit );

			// Play PAUNCH hit animation
			((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::PunchNeutral, iAnimationSubset, 0, 0 );
		}
		else
		{
			// Set cooldown if miss
			SetCooldown( x, fCooldownOnMiss );
			// Play PAUNCH miss animation
			((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::PunchNeutralMiss, iAnimationSubset, 0, 0 );
		}
	}
	return false;
}

void ItemBlockPuncher::Attack ( XTransform& )
{
	if ( !pOwner ) {
		Debug::Console->PrintError( "Itempuncher used with no owner!" );
		return;
	}

	// Notify attack
	if ( pOwner->IsCharacter() ) {
		((CCharacter*)(pOwner))->OnMeleeAttack();
	}

	// Do a raycast
	Ray viewRay = pOwner->GetEyeRay();
	RaycastHit	result;
	BlockTrackInfo	block;
	CGameBehavior*	hitBehavior;
	block.block.block = Terrain::EB_NONE;

	//if ( Raytracer.Raycast( viewRay, fMaxToolRange, &result, &block, 1|2|4 ) )
	Item::HitType hittype = Caster::Raycast( viewRay, fMaxToolRange, &result, &block, &hitBehavior, NULL, 0, pOwner );
	if ( hittype != Item::HIT_NONE && hittype != Item::HIT_UNKNOWN )
	{
		// Based on the result and block hit, check if is in list/queue
		// CVoxelTerrain::terrainList[0] is always guarenteed to be the current active terrain, even if there are multiple terrains existing.
		if ( hittype == Item::HIT_TERRAIN )
		{
			if (( block.block.block == Terrain::EB_NONE )||( block.block.block == Terrain::EB_WATER )) {
				// Set cooldown if miss
				//SetCooldown( x, fCooldownOnMiss );
				return;
			}
			bool found = false;
			// First check the queue for the specific terrain hit
			for ( std::list<HitPartInfo>::iterator it = hitList.begin(); it != hitList.end(); it++ )
			{
				// If it matches, move it to the front
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
				result, CFXMaterialHit::HT_COLLIDE );
			newHitEffect->RemoveReference();

			// Drain stamina and health for this nonsense
			if ( pOwner && ( pOwner->GetTypeName() == "CPlayer" ) )
			{
				CAfterPlayer*	pPlayer = (CAfterPlayer*)pOwner;
				Damage dmg;
				dmg.amount = fabs(0.2f * cBlockHardness[hitList.front().blockInfo.block.block]);
				dmg.type = DamageType::Crush|DamageType::Reflect;
				dmg.stagger_chance = 0;
				pPlayer->OnDamaged( dmg );
				pPlayer->GetPlayerStats()->stats->fStamina -= fabs(0.8f * cBlockHardness[hitList.front().blockInfo.block.block]);

				// TODO: if stamina is now too low, then play OUCH animation.
				if ( pPlayer->GetPlayerStats()->stats->fStamina < 8.0f )
				{
					((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::PunchNeutralBreak, iAnimationSubset, 0, 0 );
					SetCooldown( Item::UPrimary, fCooldownOnMiss );
				}
			}

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
					((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::PunchNeutralBreak, iAnimationSubset, 0, 0 );
					SetCooldown( Item::UPrimary, fCooldownOnMiss );
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
						((CCharacter*)pOwner)->OnGainExperience( Experience(4,DscMining) );
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
			else {
				// If it's the first hit, then destroy some grass.
				//if ( hitList.front().hitCount == 1 ) {
				// If it's a top hit, then destroy some grass.
				if ( result.hitNormal.z > 0.5f ) {
					//hitList.front().blockInfo.pBoob->pGrass->BreakGrass( hitList.front().blockInfo.pBlock );
					CTerrainAccessor accessor;
					accessor.DestroyGrass( hitList.front().blockInfo );
				}
			}
		}
		else if ( hittype == Item::HIT_TREE )
		{
			bool found = false;
			
			CTreeBase* pHitTree = (CTreeBase*)hitBehavior;
			TreePart* pTreePartHit = pHitTree->GetPartClosestTo( result.hitPos );

			std::cout << "Hit a tree." << " part: " << pTreePartHit << " tree: " << pHitTree << std::endl;

			//DebugD::DrawLine( pTreePartHit->shape.pos+pHitTree->transform.position, result.hitPos );
			// Do material effects
			CFXMaterialHit* newHitEffect = new CFXMaterialHit(
				*PhysMats::Get(PhysMats::MAT_Wood),
				result, CFXMaterialHit::HT_HIT );
			newHitEffect->RemoveReference();

			// Drain stamina and health for this nonsense
			if ( pOwner && ( pOwner->GetTypeName() == "CPlayer" ) )
			{
				CAfterPlayer*	pPlayer = (CAfterPlayer*)pOwner;
				Damage dmg;
				//dmg.amount = 9.0f;
				//dmg.type = DamageType::Crush|DamageType::Reflect;
				dmg = pHitTree->GetPunchDamage();
				pPlayer->OnDamaged( dmg );
				pPlayer->GetPlayerStats()->stats->fStamina -= dmg.amount*0.9f;

				// TODO: if stamina is now too low, then play OUCH animation.
				if ( pPlayer->GetPlayerStats()->stats->fStamina < dmg.amount*0.9f )
				{
					((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::PunchNeutralBreak, iAnimationSubset, 0, 0 );
					SetCooldown( Item::UPrimary, fCooldownOnMiss );
				}
			}

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
		/*else
		{
			CGameObject* pHitObject = ((CRigidBody*)result.pHitBehavior)->GetOwner();
			if ( pHitObject )
			{
				if ( pHitObject->GetBaseClassName() == "CActor_Character" )
				{
					CCharacter* pHitCharacter = (CCharacter*)pHitObject;
					if ( pHitCharacter != pOwner )
					{
						// Do minor damage
						Damage meleeDamage;
						meleeDamage.amount	= 2;
						meleeDamage.type	= DamageType::Phys;
						meleeDamage.actor	= pOwner;
						pHitCharacter->OnDamaged( meleeDamage );
					}
				}
				else
				{
					cout << "Punched a "
						<< ((CRigidBody*)result.pHitBehavior)->GetOwner()->GetTypeName() << " ("
						<< ((CRigidBody*)result.pHitBehavior)->GetOwner()->GetBaseClassName() << ") "
						<< endl;
				}
			}
			else
			{
				cout << "Punched a rigidbody with no associated Gamebehavior!" << endl;
			}
		}*/
		else if ( hittype == Item::HIT_ACTOR || hittype == Item::HIT_CHARACTER )
		{
			CActor* pHitCharacter = (CCharacter*)hitBehavior;
			if ( pHitCharacter != pOwner )
			{
				// Do minor damage
				Damage meleeDamage;
				meleeDamage.amount	= 2;
				meleeDamage.type	= DamageType::Physical;
				meleeDamage.actor	= pOwner;
				meleeDamage.source	= pOwner->GetEyeRay().pos;
				meleeDamage.direction = pOwner->GetEyeRay().dir;

				if ( pOwner ) pOwner->OnDealDamage( meleeDamage, pHitCharacter );
				pHitCharacter->OnDamaged( meleeDamage );

				std::cout << "Punched a "
					<< hitBehavior->GetTypeName() << " ("
					<< hitBehavior->GetBaseClassName() << ") "
					<< " for " << meleeDamage.amount << " damage" << std::endl;
			}
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
		//SetCooldown( x, fCooldownOnHit );

		// Play PAUNCH hit animation
		//pOwner->PlayItemAnimation( "punch_neutral" );
	}
	else
	{
		// Set cooldown if miss
		//SetCooldown( x, fCooldownOnMiss );
		// Play PAUNCH miss animation
		//pOwner->PlayItemAnimation( "punch_neutral_miss" );
	}
}


// Returns amount of hits needed to destroy something with the given hardness
short ItemBlockPuncher::GetHitCount ( char hardness )
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