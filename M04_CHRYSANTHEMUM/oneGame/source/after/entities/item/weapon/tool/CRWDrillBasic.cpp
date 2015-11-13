
#include "CRWDrillBasic.h"
#include "after/entities/character/CAfterPlayer.h"
#include "after/interfaces/CCharacterMotion.h"
#include "engine/physics/motion/CRigidbody.h"
#include "core/time/time.h"
#include "after/types/terrain/BlockType.h"
#include "renderer/logic/model/CModel.h"

CRWDrillBasic::CRWDrillBasic ( tMeleeWeaponProperties& inProps )
	: CBaseRandomMelee( inProps, ItemData() )
{
	holdType = Item::ChannelMelee;

	// Initialize Default Values
	iHitMutliplier	= 2;
	fCooldownOnHit	= 0.1f;
	fCooldownOnMiss = 0.7f;
}
CRWDrillBasic::CRWDrillBasic ( void )
	: CBaseRandomMelee( tMeleeWeaponProperties(), ItemData() )
{
	holdType = Item::ChannelMelee;
	// Initialize Default Values
	iHitMutliplier	= 2;
	fCooldownOnHit	= 0.1f;
	fCooldownOnMiss = 0.7f;
}

CRWDrillBasic::~CRWDrillBasic ( void )
{
	;
}

void CRWDrillBasic::SetBlockHardness ( void )
{
	// Set Block Strengths
	cBlockHardness[Terrain::EB_DIRT]		= 4;
	cBlockHardness[Terrain::EB_GRASS]		= 5;
	cBlockHardness[Terrain::EB_SAND]		= 7;
	cBlockHardness[Terrain::EB_CLAY]		= 5;
	cBlockHardness[Terrain::EB_WOOD]		= 7;
	cBlockHardness[Terrain::EB_WIRE]		= 1;
	cBlockHardness[Terrain::EB_STONE]		= 10;
	cBlockHardness[Terrain::EB_STONEBRICK]	= 10;
}

void CRWDrillBasic::Generate ( void )
{
	CBaseRandomItem::Generate();

	string drillModel = "models/items/drill_base.FBX";
	pModel = new CModel( drillModel );
}

bool CRWDrillBasic::Use ( int x )
{
	if ( !CanUse( x ) )
		return true;
	if ( x == Item::UPrimary )
	{
		CActor* pHitCharacter;
		pHitCharacter = FivePointCollision();
		if ( pHitCharacter )
		{
			return true;
		}
		else
		{
			if ( DoBlockDestruction( x ) )
			{
				bHitBlock = true;	// Flag we can drag play along
			}
			return true;
		}
	}
	return false;
}

void CRWDrillBasic::Update ( void )
{
	// Make sure to call inherited Update
	CBaseRandomMelee::Update();

	// Check if drill hit a block far back
	if ( bHitBlock )
	{
		// Carry player along with the block 
		if ( pOwner && ( pOwner->GetTypeName() == "CPlayer" ) )
		{
			CAfterPlayer*	pPlayer = (CAfterPlayer*)pOwner;
			if ( (pPlayer->GetMotionState()->bIsProne) || (pPlayer->GetMotionState()->bIsCrouching) )
			{
				pPlayer->GetMotionState()->m_rigidbody->SetVelocity( Vector3d( 0,0,0 ) );
				pPlayer->GetMotionState()->m_rigidbody->AddToPosition( pPlayer->GetEyeRay().dir * Time::deltaTime * 1.3f );
			}
		}
		// Check to see to stop drawing player along
		if ( CanUse( Item::UPrimary ) )
		{
			bHitBlock = false;
		}
	}
}