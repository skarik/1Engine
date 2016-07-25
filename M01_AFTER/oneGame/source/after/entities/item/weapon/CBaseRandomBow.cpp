
#include "CBaseRandomBow.h"
#include "core-ext/animation/CAnimation.h"
#include "engine/physics/raycast/Raycaster.h"
#include "after/entities/character/CCharacter.h"
//#include "CFXMaterialHit.h"

#include "after/entities/projectile/projectile/ProjectileArrow.h"
#include "after/entities/item/ammo/AmmoArrow.h"

#include "after/states/inventory/CPlayerInventory.h"

#include "core/math/random/Random.h"

#include "after/entities/item/skill/CSkill.h"
#include "renderer/logic/model/CModel.h"
#include "core-ext/animation/CAnimAction.h"

#include "renderer/material/glMaterial.h"

CBaseRandomBow::CBaseRandomBow( tBowWeaponProperties &newProps, const WItemData &wdat )
	: CBaseRandomItem( wdat )
{
	weapon_stats = newProps;
	weaponItemData.iHands = weapon_stats.hands;

	//InitializeBlockDestruction();
	//SetBlockHardness();
	//bHardnessSet = false;

	//vHoldingOffset = Vector3d( 0.0f, 0.35f, -0.05f ); // Move hand to the handle
	//vHoldingOffset = Vector3d( 0.0f, 0.35f, -0.00f ); // Move hand to the handle

	pauseTime = 0;

	currentAmmo = NULL;

	mAnimationIdleAction = NPC::ItemAnim::INVALID;
}
CBaseRandomBow::~CBaseRandomBow ( void )
{
	;
}
CWeaponItem& CBaseRandomBow::operator= ( const CWeaponItem * original )
{
	CBaseRandomBow* oitem = (CBaseRandomBow*)original;
	weapon_stats = oitem->weapon_stats;
	weaponItemData.iHands = weapon_stats.hands;

	return CBaseRandomItem::operator= ( original );
}

// Serialization
void CBaseRandomBow::serialize ( Serializer & ser, const uint ver )
{
	CBaseRandomItem::serialize(ser,ver);
	ser & weapon_stats.range;
	ser & weapon_stats.damage;
	ser & weapon_stats.hands;
	ser & weapon_stats.speed;
	ser & weapon_stats.pspeed;
	weaponItemData.iHands = weapon_stats.hands;
}


bool CBaseRandomBow::Use ( int x )
{
	if ( !CanUse( 0 ) )
		return true;
	if ( x == Item::USecondary ) {
		bool canPullback = true;
		// Disable pullback if no stamina
		CharacterStats* stats = pOwner->GetCharStats();
		if ( stats ) {
			if ( stats->fStamina < 1.0f ) {
				canPullback = false;
			}
		}
		// If can pullback, then pullback
		if ( canPullback )
		{
			pauseTime = 2;
			tension += Time::deltaTime * weapon_stats.speed;
			if ( tension > 1 ) {
				tension = 1;
			}
		}
		else { // Can't pullback, but still trying to pullback, just subtract stamina
			if ( stats ) {
				stats->fStamina -= Time::deltaTime * 2.0f;
			}
		}

		if ( canPullback ) {
			// Now, find any [first] valid modifier
			CSkill* targetModifier = NULL;
			if ( pOwner ) {
				for ( short i = 0; i < pOwner->GetInventory()->GetCurrentEquippedCount(); ++i ) {
					CWeaponItem* currentTarget = pOwner->GetInventory()->GetCurrentEquipped(i);
					if ( currentTarget && currentTarget != this ) {
						if ( currentTarget->GetIsSkill() )
						{
							CSkill* currentSkill = (CSkill*)currentTarget;
							if ( currentSkill->IsModifier( this ) && currentSkill->CanUse(Item::UModifier) && currentSkill->CanCast() ) {
								targetModifier = currentSkill;
								break;
							}
						}
					}
				}
			}
			if ( targetModifier ) {
				targetModifier->Use( Item::UModifierPrepare );
			}
		}
	}
	/*else if ( x == UPrimary ) {
		if ( pauseTime > 0 ) {
			// Do ranged attack
			// Search character's inventory for ammo, don't do anything if no arrow
			if ( hasAmmo ) {
				if ( currentAmmo ) {
					currentAmmo->SetStackSize( currentAmmo->GetStackSize() - 1 );
					// Play animations
					if ( pModel ) {
						if ( pModel->GetAnimation() ) {
							CAnimAction* action = pModel->GetAnimation()->FindAction( "attack" );
							action->layer = 2;
							action->Play();
						}
					}

					if ( pOwner ) {
						//pOwner->PlayItemAnimation( "bow_fire" );
						((CCharacter*)pOwner)->OnRangedAttack();
					}

					// Now, find any [first] valid modifier
					CSkill* targetModifier = NULL;
					if ( pOwner ) {
						for ( short i = 0; i < pOwner->GetInventory()->GetCurrentEquippedCount(); ++i ) {
							CWeaponItem* currentTarget = pOwner->GetInventory()->GetCurrentEquipped(i);
							if ( currentTarget && currentTarget != this ) {
								if ( currentTarget->GetIsSkill() )
								{
									CSkill* currentSkill = (CSkill*)currentTarget;
									if ( currentSkill->IsModifier( this ) && currentSkill->CanUse(UModifier) && currentSkill->CanCast() ) {
										targetModifier = currentSkill;
										break;
									}
								}
							}
						}
					}

					if ( targetModifier == NULL )
					{	// Create projectile here
						SetCooldown( 0, 0.6f / weapon_stats.speed );
	
						ProjectileArrow*	tempArrow;
						Ray		aimDir;
						ftype	fireSpeed;
						Damage	arrowDamage;
	
						aimDir = pOwner->GetAimRay( transform.position );
						aimDir.dir = ( aimDir.dir+(Random.PointOnUnitSphere()*0.15f*(1.03f-tension)) ).normal(); // Add randomness
						fireSpeed = weapon_stats.pspeed * 0.4f * ((0.1f+tension)/1.1f);

						arrowDamage.amount	= weapon_stats.damage * ((0.2f+tension)/1.2f);
						arrowDamage.type	= DamageType::Pierce;
						
						tempArrow = new ProjectileArrow( aimDir, fireSpeed*28.0f );
						tempArrow->SetDamping( 0.04f + (1-tension)*0.16f, 0.95f + (1-tension)*0.15f );
						tempArrow->SetOwner( pOwner );
						tempArrow->SetDamage( arrowDamage );
					}
					else {
						if ( targetModifier->Use( UModifier ) ) {
							SetCooldown( 0, 0.6f / weapon_stats.speed );
						}
					}
				}
			}
		}
		else {
			// Do melee attack
			// (copy from melee item?)
		}
	}*/

	return false;
}

void CBaseRandomBow::StartUse ( int x )
{
	if ( x == Item::UPrimary ) {
		if ( pauseTime > 0 ) {
			// Do ranged attack
			// Search character's inventory for ammo, don't do anything if no arrow
			if ( hasAmmo ) {
				if ( currentAmmo ) {
					currentAmmo->SetStackSize( currentAmmo->GetStackSize() - 1 );
					// Play animations
					/*if ( pModel ) {
						if ( pModel->GetAnimation() ) {
							CAnimAction* action = pModel->GetAnimation()->FindAction( "attack" );
							action->layer = 2;
							action->Play();
						}
					}*/
					// Perform ranged attack call
					if ( pOwner ) {
						((CCharacter*)pOwner)->OnRangedAttack();
					}

					// Now, find any [first] valid modifier
					CSkill* targetModifier = NULL;
					if ( pOwner ) {
						for ( short i = 0; i < pOwner->GetInventory()->GetCurrentEquippedCount(); ++i ) {
							CWeaponItem* currentTarget = pOwner->GetInventory()->GetCurrentEquipped(i);
							if ( currentTarget && currentTarget != this ) {
								if ( currentTarget->GetIsSkill() )
								{
									CSkill* currentSkill = (CSkill*)currentTarget;
									if ( currentSkill->IsModifier( this ) && currentSkill->CanUse(Item::UModifier) && currentSkill->CanCast() ) {
										targetModifier = currentSkill;
										break;
									}
								}
							}
						}
					}

					if ( targetModifier == NULL )
					{	// Create projectile here
						SetCooldown( 0, 0.6f / weapon_stats.speed );
	
						ProjectileArrow*	tempArrow;
						Ray		aimDir;
						ftype	fireSpeed;
						Damage	arrowDamage;
	
						aimDir = pOwner->GetAimRay( transform.position );
						aimDir.dir = ( aimDir.dir+(Random.PointOnUnitSphere()*0.15f*(1.03f-tension)) ).normal(); // Add randomness
						fireSpeed = weapon_stats.pspeed * 0.4f * ((0.1f+tension)/1.1f);

						arrowDamage.amount	= weapon_stats.damage * ((0.2f+tension)/1.2f);
						arrowDamage.type	= DamageType::Pierce;
						
						tempArrow = new ProjectileArrow( aimDir, fireSpeed*27.0f );
						tempArrow->SetDamping( 0.05f + (1-tension)*0.16f, 0.97f + (1-tension)*0.15f );
						tempArrow->SetOwner( pOwner );
						tempArrow->SetDamage( arrowDamage );
					}
					else {
						if ( targetModifier->Use( Item::UModifier ) ) {
							SetCooldown( 0, 0.6f / weapon_stats.speed );
						}
					}

					// Take away stamina here
					/*CharacterStats* stats = pOwner->GetCharStats();
					if ( stats )
					{
						//ftype chargeAmount = std::min<ftype>( 1.0f, fChargeTime/weapon_stats.charge_full_time );
						//stats->fStamina -= (chargeAmount+1.0f)*weaponItemData.fWeight*3.0f;
					}*/
				}
			}
		}
		else {
			// Do melee attack
			// (copy from melee item?)
		}
	}
}

void CBaseRandomBow::Update ( void )
{
	// Call base update
	CBaseRandomItem::Update();

	// Search the inventory for ammo
	hasAmmo = false;
	currentAmmo = NULL;
	if ( pOwner )
	{
		CPlayerInventory* inv = (CPlayerInventory*)pOwner->GetInventory();
		if ( inv ) {
			currentAmmo = (AmmoArrow*)inv->FindItem( 11 ); 
			if ( currentAmmo ) {
				hasAmmo = true;
			}	
		}
	}
	//
	if ( holdState == Item::Holding )
	{
		// Decrement pause time, and then relax bow.
		if ( --pauseTime <= 0 ) {
			pauseTime = 0;
			// Relax bow
			tension -= Time::deltaTime * 2.0f * weapon_stats.speed;
			if ( tension < 0 ) {
				tension = 0;
			}
			SetViewAngleOffset( -tension * 20.0f );
		}
		//else {
		//}
		if ( tension > 0.01f ) {
			iAnimationSubset = 5;
			// If not pulling bow, then do pull action
			if ( pModel ) {
				if ( pModel->GetAnimation() ) {
					CAnimAction* action = pModel->GetAnimation()->FindAction( "pull" );
					if ( action ) {
						action->Play( 1.0f, 0.2f );
						action->framesPerSecond = 0.01f;
						action->frame = (action->GetLength()-1.2f) * tension;
					}
					else {
						std::cout << "No action called 'pull'" << std::endl;
					}
				}
			}
			if ( pOwner ) {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Charge, 1, mHand, tension );
			}
			// Take away stamina
			CharacterStats* stats = pOwner->GetCharStats();
			if ( stats ) {
				stats->fStamina -= Time::deltaTime * 4.0f * tension;
			}
			SetViewAngleOffset( -tension * 20.0f );
		}
		else {
			// Play animations
			if ( pModel ) {
				if ( pModel->GetAnimation() ) {
					CAnimAction* action = pModel->GetAnimation()->FindAction( "ref" );
					action->Play(Time::deltaTime);
				}
			}
			if ( pOwner ) {
				((CCharacter*)pOwner)->PlayItemAnimation( NPC::ItemAnim::Idle, 1, mHand, 0 );
			}
		}//
	}
	else
	{
		iAnimationSubset -= 1;
		tension = 0;
	}//
}

void CBaseRandomBow::LateUpdate ( void )
{
	CBaseRandomItem::LateUpdate();

	if ( holdState == Item::Holding && CanUse(0) )
	{
		if ( currentAmmo ) {
			if ( iAnimationSubset > 0 )
			{
				// If has arrow, show arrow
				currentAmmo->SetHoldState( Item::UserSet );
				currentAmmo->transform.position = transform.position + transform.rotation*( Vector3d::forward*((1-tension)*1.8f+0.2f) - Vector3d(0,-0.07f,0.2f) );
				currentAmmo->transform.rotation = transform.rotation * Rotator( 90,0,0 );
			}
			else
			{
				XTransform hand2 = pOwner->GetHoldTransform( mHand%2 );
				currentAmmo->transform.rotation = hand2.rotation * Rotator(0,180,0);
				currentAmmo->transform.position = hand2.position + hand2.rotation*Vector3d( 1,0,0 );
			}
		}
	}
	else
	{
		if ( currentAmmo ) {
			if ( currentAmmo->GetHoldState() == Item::UserSet ) {
				currentAmmo->SetHoldState( Item::Hidden );
			}
		}
	}//
}


// ================================================
// HUD Element Rendering
// ================================================

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "core/system/Screen.h"
#include "renderer/texture/CBitmapFont.h"
#include "core/settings/CGameSettings.h"
//#include "after/entities/item/CWeaponItem.h"

// Draw HUD elements
void CBaseRandomBow::OnDrawHUD ( const WReticleStyle& n_reticleStyle )
{
	// Draw default reticle, scale with tension
	/*n_reticleStyle.matReticle->m_diffuse = Color( 0.0f,0.0f,0.0f,tension );
	n_reticleStyle.matReticle->bindPass(0);
		GLd.DrawSet2DMode( GLd.D2D_FLAT );
		GLd.DrawRectangle(
			Screen::Info.width/2-2 - int((1-tension)*20),
			Screen::Info.height/2-2 - int((1-tension)*20),
			4 + int((1-tension)*40),
			4 + int((1-tension)*40)
		);*/
	Vector2d centerPosition ( Screen::Info.width/2.0f, Screen::Info.height/2.0f );
	Vector2d reticleHalfsize ( 16,16 );
	reticleHalfsize *= CGameSettings::Active()->f_cl_ReticleScale;

	n_reticleStyle.matReticle->m_diffuse = Color( 1.0f,1.0f,1.0f,(1.0f-tension)*0.6f );
	n_reticleStyle.matReticle->bindPass(0);

	GLd_ACCESS;

	GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushColor( Color(1,1,1,1) );
		GLd.P_PushTexcoord( Vector2d(1,0)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,-reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(1,1)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(2,0)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,-reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(2,1)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,reticleHalfsize.y) );
	GLd.EndPrimitive();

	n_reticleStyle.matReticle->m_diffuse = Color( 1.0f,1.0f,1.0f,tension );
	n_reticleStyle.matReticle->bindPass(0);

	reticleHalfsize = Vector2d ( 16 + ((1-tension)*40), 16 + ((1-tension)*40) );
	reticleHalfsize *= CGameSettings::Active()->f_cl_ReticleScale;

	GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushColor( Color(1,1,1,1) );
		GLd.P_PushTexcoord( Vector2d(0,2)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,-reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(0,3)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(1,2)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,-reticleHalfsize.y) );
		GLd.P_PushTexcoord( Vector2d(1,3)/8 );
		GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,reticleHalfsize.y) );
	GLd.EndPrimitive();
}

// Randomized weapons get a slightly different tooltip that shows their stats.
void CBaseRandomBow::DrawTooltip ( const Vector2d & drawPos, const WTooltipStyle & style ) 
{
	GLd_ACCESS;

	TooltipDrawBackground( drawPos,style );
	TooltipDrawName( drawPos,style );
	TooltipDrawRarity( drawPos,style );
	style.matFont->setTexture( 0, style.fontTexture );
	style.matFont->bindPass(0);
	GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.08f, "DMG: %.0f", weapon_stats.damage );
	GLd.DrawAutoText( drawPos.x+0.12f, drawPos.y+0.08f, "MOV: %.2f", weapon_stats.pspeed );
	GLd.DrawAutoText( drawPos.x+0.02f, drawPos.y+0.11f, "SPD: %.2f", weapon_stats.speed );
	GLd.DrawAutoText( drawPos.x+0.12f, drawPos.y+0.11f, "RNG: %.0f", weapon_stats.range );
	GLd.DrawAutoText( drawPos.x+0.12f, drawPos.y+0.14f, "HND: %d", weapon_stats.hands );

	TooltipDrawDescription( drawPos+Vector2d(0,0.09f),style );
	TooltipDrawPicture( drawPos,style );
}
