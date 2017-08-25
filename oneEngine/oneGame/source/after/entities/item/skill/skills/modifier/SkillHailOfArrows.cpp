
#include "SkillHailOfArrows.h"

#include "after/entities/projectile/projectile/ProjectileArrow.h"
#include "after/states/projectileeffect/effects/PEffectBouncing.h"

#include "after/entities/item/weapon/CBaseRandomBow.h"
#include "core/math/random/Random.h"

#include "renderer/object/shapes/CRenderPlane.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"

#include "engine/physics/raycast/Raycaster.h"

SkillHailOfArrows::SkillHailOfArrows ( void ) : CSkill( ItemData() )
{
	mana_cost	= 8;
	stamina_cost= 12;
	cooldown	= 11.0f;
	passive		= false;

	holdType = Item::HoldType::EmptyHanded;

	
	// Create AoE model
	mAoeModel = new CRenderPlane( 20.0f, 20.0f );
	glMaterial* aoemat = new glMaterial;
	aoemat->setTexture( 0, new CTexture( "textures/areas/skillzone_default.png" ) );
	aoemat->m_emissive = Color( 0, 0, 0 );
	aoemat->m_emissive = Color( 0.85f, 0.9f, 1.2f );
	aoemat->passinfo.push_back( glPass() );
	aoemat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	aoemat->passinfo[0].shader = new glShader( "shaders/particles/colorBlendedSoftAdd.glsl" );
	aoemat->removeReference();
	mAoeModel->SetMaterial( aoemat );

	mAoeAlpha = -0.1f;
	mAoeFadeTime = -0.1f;
}

SkillHailOfArrows::~SkillHailOfArrows ( void )
{
	delete_safe( mAoeModel );
}

// Item cast
bool SkillHailOfArrows::Use ( int x )
{
	if ( x == Item::UModifier ) {
		// Cast from mana
		CastMana();
		// Set cooldown
		SetCooldown( x, cooldown );

		// Create projectile
		ProjectileArrow*	tempArrow;
		Ray		aimDir;
		ftype	fireSpeed;
		Damage	arrowDamage;
	
		ftype tension = 1.0f;

		aimDir = pOwner->GetAimRay( transform.position );
		aimDir.dir = ( aimDir.dir+(Random.PointOnUnitSphere()*0.15f*(1.03f-tension)) ).normal(); // Add randomness
		fireSpeed = mBow->weapon_stats.pspeed * 0.4f * ((0.1f+tension)/1.1f) * 0.8f; // Slightly arrow than normal shot.

		arrowDamage.amount	= mBow->weapon_stats.damage * ((0.2f+tension)/1.2f) * 0.8f; //slightly reduced damage, but has a DoT
		arrowDamage.type	= DamageType::Pierce | DamageType::Magical;
						
		tempArrow = new ProjectileArrow( aimDir, fireSpeed*28.0f );
		tempArrow->SetDamping( 0.04f + (1-tension)*0.16f, 0.95f + (1-tension)*0.15f );
		tempArrow->SetOwner( pOwner );
		tempArrow->SetDamage( arrowDamage );
		tempArrow->Effects()->Add( new PEffectBouncing(3) );

		return true;
	}
	else if ( x == Item::UModifierPrepare ) {
		mAoeFadeTime = 0.3f;
		mAoeAlpha = 1.0f;
	}
	return false;
}
// Is modifier for bow items
bool SkillHailOfArrows::IsModifier ( const CWeaponItem* item )
{
	if ( item->GetItemData()->eTopType == WeaponItem::ItemBow ) {
		mBow = (CBaseRandomBow*)item;
		return true;
	}
	else {
		return false;
	}
}

// Update visual for AoE
void SkillHailOfArrows::Update ( void )
{
	if ( mAoeAlpha > 0 ) {
		mAoeModel->SetVisible( true );

		// Raytrace to the terrain for the AoE model
		if ( mAoeFadeTime > 0 ) {
			Ray		aimDir;
			aimDir = pOwner->GetAimRay( pOwner->GetEyeRay().pos );
			RaycastHit hitResult;
			if ( Raycaster.Raycast( aimDir, 120.0f, &hitResult, Physics::GetCollisionFilter( Layers::PHYS_CHARACTER ), pOwner ) ) {
				mAoeModel->transform.position = hitResult.hitPos + hitResult.hitNormal*0.6f;
			}
		}

		// Set the AoE alpha
		mAoeModel->GetMaterial()->m_diffuse.alpha = mAoeAlpha;
	}
	else {
		/*if ( mAoeModel ) {
			delete mAoeModel;
			mAoeModel = NULL;
		}*/
		mAoeModel->SetVisible( false );
	}

	// Fade out the AoE over time
	mAoeFadeTime -= Time::deltaTime;
	if ( mAoeFadeTime <= 0 ) {
		mAoeAlpha -= Time::deltaTime;
	}
	
}