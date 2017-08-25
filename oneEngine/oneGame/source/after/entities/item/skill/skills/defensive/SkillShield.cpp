

#include "SkillShield.h"
#include "core/time/time.h"
#include "after/states/CharacterStats.h"
#include "after/entities/character/CCharacter.h"
#include "renderer/object/mesh/CMesh.h"

#include "engine/physics/material/physMaterial.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"

SkillShield::SkillShield ( void ) : CSkill( ItemData() )
{
	mana_cost	= 20;
	cooldown	= 3.0f;
	passive		= false;

	m_health	= false;
	m_shielding = false;

	m_model = NULL;
}

SkillShield::~SkillShield ( void )
{
	ShieldEffectDestroy();
	if ( pOwner && pOwner->IsCharacter() ) {
		CCharacter* pOwnerChar = (CCharacter*)pOwner;
		pOwnerChar->SetShield( this );
	}
}

void SkillShield::ShieldEffectUpdate ( void )
{
	if ( !m_model ) {
		m_model = new CModel( "models/geosphere.FBX" );
		glMaterial* copyMaterial = m_model->GetMesh(uint(0))->GetMaterial()->copy();
		copyMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		copyMaterial->passinfo[0].m_blend_mode = Renderer::BM_ADD;
		copyMaterial->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
		copyMaterial->setTexture( 0, new CTexture( "textures/caustics.png" ) );
		copyMaterial->m_diffuse = Color( 0.1,0.1,0.1,1.0 );
		m_model->GetMesh(uint(0))->SetMaterial( copyMaterial );
	}
	glMaterial* targetMaterial = m_model->GetMesh(uint(0))->GetMaterial();
	targetMaterial->m_diffuse = Color( 0.1,0.1,0.1,1.0 );

	m_model->transform.position = pOwner->transform.position + Vector3d(0,0,3);
	m_model->transform.scale = Vector3d(3,3,3);

	for ( uint i = 0; i < m_aspects.size(); ++i )
	{
		// perform effect edits based on the aspect
		if ( m_aspects[i].element & DamageType::Fire ) {
			targetMaterial->m_diffuse = Color( 0.3,0.2,0.1,1.0 );
		}
	}
}
void SkillShield::ShieldEffectDestroy ( void )
{
	if ( m_model ) {
		delete m_model;
		m_model = NULL;
	}
}

void SkillShield::Update ( void )
{
	CSkill::Update();

	// Shield always cost 50% of mana
	if ( pOwner ) {
		mana_cost = pOwner->GetCharStats()->fManaMax * 0.5f;
	}

	// If shield is up
	if ( m_shielding )
	{
		// Create the shield model
		ShieldEffectUpdate();

		if ( m_health <= 0 )
		{
			// Shield has been taken down. Set cooldowns.
			SetCooldown( 0,cooldown );
			m_shielding = false;
		}
	}
	else
	{
		ShieldEffectDestroy();
	}
}

// Item cast
bool SkillShield::Use ( int use ) 
{
	if ( use == Item::UPrimary )
	{
		if ( !m_shielding )
		{
			if ( CanUse(0) && (CanCast()) )
			{
				// Turn on the shield, subtracting shield
				CastMana();
				m_shielding = true;

				// Channelling the shield
				isChannelling = true;
				isCasting = true;

				// Create the shield health with base of 50 plus int*2
				if ( pOwner->IsCharacter() )
				{
					CCharacter* pOwnerChar = (CCharacter*)pOwner;
					m_health = 50.0f + (pOwnerChar->GetCharStats()->iIntelligence * 2);
					m_max_health = m_health;

					pOwnerChar->SetShield( this );
				}
			}
		}
		else
		{
			// Shield is already up, but we're putting power into it
			if ( CanUse(0) )
			{
				// Check for mana to replenish shield (and if it can accept any more power)
				if ( (m_health < m_max_health) && pOwner->IsCharacter() )
				{
					CCharacter* pOwnerChar = (CCharacter*)pOwner;
					if ( pOwnerChar->GetCharStats()->fMana > 0 )
					{
						// Replenish shield
						pOwnerChar->GetCharStats()->fMana -= Time::deltaTime*20;
						m_health += Time::deltaTime*20;
						if ( m_health > m_max_health ) {
							m_health = m_max_health;
						}
					}
				}
				// Still casting it.
				isCasting = true;
			}
		}
	}
	return false;
}
void SkillShield::EndUse ( int use )
{
	if ( use == Item::UPrimary )
	{
		isChannelling = false;
		isCasting = false;
	}
}


// Run through damages
Damage SkillShield::OnDamaged ( const Damage& inDamage, DamageFeedback* outFeedback )
{
	Damage resultDamage = inDamage;

	if ( m_shielding && pOwner->IsCharacter() )
	{
		CCharacter* pOwnerChar = (CCharacter*)pOwner;
		ftype apparentDamage = resultDamage.amount;

		// Increase damage if it's physical
		if (!( inDamage.type & DamageType::Magical )) {
			apparentDamage *= 4; // 400% melee damage
		}

		// Run through the shield aspects (some incoming damages can damage a shield)

		// Now apply damage types
		if ( apparentDamage > m_health )
		{
			m_health -= apparentDamage;
			resultDamage.amount = std::min<ftype>( resultDamage.amount*0.1f, pOwnerChar->GetCharStats()->fHealthMax*0.1f );
		}
		else
		{
			m_health -= apparentDamage;
			resultDamage.amount = 0;
			if ( outFeedback ) {
				outFeedback->material = PhysMats::Get(PhysMats::MAT_Sand);
			}
		}
	}
	
	return resultDamage;
}


// ChannelCombine. Combines two casting skills for an effect, except the callee is channelling.
// Returns true if the skill doesn't block non-instant casts.
// Returns false if the channelling skill should stop skills. Generally, all offensive magic spells are non-instants.
bool SkillShield::ChannelCombine ( CSkill* callingSkill ) 
{
	short skillID = -callingSkill->GetID();
	if ( skillID == 12 )
	{
		// Add fire to the aspects (first search for fire)
		for ( uint i = 0; i < m_aspects.size(); ++i )
		{
			if ( m_aspects[i].source == skillID )
			{
				m_aspects[i].element = DamageType::Fire;
				m_aspects[i].strength = 1.0f;
				return false;
			}
		}
		aspect_t newAspect;
		newAspect.element = DamageType::Fire;
		newAspect.source = skillID;
		newAspect.strength = 1.0f;
		m_aspects.push_back( newAspect );

		return false;
	}

	return true;
}