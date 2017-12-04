#include "PlayerMage.h"

#include "core/math/Math.h"
#include "render2d/object/sprite/CEditableRenderable2D.h"

#include "ld40/interfaces/CameraControllerPlatformer.h"
#include "ld40/interfaces/CharacterControllerPlatformer.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(player_mage,M04::PlayerMage);

PlayerMage::PlayerMage ( void )
	: PlayerPlatformerBase()
{
	// Set up the sprite
	{
		this->SetupDepthOffset(0.0F, 0.0F);
		anim_idlePose	= this->AddFromFile(animation::kTypeIdle, 0, "sprites/ld40/mage2_idlepose.gal", "sprites/ld40/mage2_pal.gal");
		anim_idleReady	= this->AddFromFile(animation::kTypeIdle, 0, "sprites/ld40/mage2_idleready.gal", "sprites/ld40/mage2_pal.gal");
		anim_fall		= this->AddFromFile(animation::kTypeFall, 0, "sprites/ld40/mage2_fall.gal", "sprites/ld40/mage2_pal.gal");
		this->m_spriteOrigin = Vector2i( m_spriteSize.x / 2, m_spriteSize.y / 2 );
	}
}

PlayerMage::~PlayerMage ( void )
{
}


void PlayerMage::Update ( void )
{	
	PlayerPlatformerBase::Update();

	// Update animations

	if ( !motion->CheckIsOnGround() )
	{
		Play(anim_fall);
		afx_idleTime = 0;
	}
	else
	{
		afx_idleTime += Time::deltaTime;
		if (afx_idleTime < 1.0F)
		{
			Play(anim_idleReady);
		}
		else
		{
			Play(anim_idlePose);
		}
	}
}

void PlayerMage::FixedUpdate ( void )
{
	PlayerPlatformerBase::FixedUpdate();

	// Update flipstate
	
	if ( motion->CheckIsOnGround() )
	{
		if (velocity.x < 0) {
			flipstate.x = -1;
		}
		else if (velocity.x > 0) {
			flipstate.x = +1;
		}
	}
}

void PlayerMage::PostFixedUpdate ( void )
{
	PlayerPlatformerBase::PostFixedUpdate();
}