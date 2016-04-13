
#include "SpriteContainer.h"
#include "render2d/object/CRenderable2D.h"

using namespace Engine2D;


SpriteContainer::SpriteContainer ( Vector3d* position, Real* angle, Vector3d* scale )
	: CLogicObject(),
	m_sourcePosition(position), m_sourceAngle(angle), m_sourceScale(scale),
	m_sprite(NULL)
{
	;
}
SpriteContainer::~SpriteContainer ( void )
{
	if ( m_sprite != NULL )
	{
		delete_safe(m_sprite);
	}
}

void SpriteContainer::PreStep ( void )
{
	// Update sprite position
	if ( m_sprite != NULL )
	{
		if ( m_sourcePosition != NULL ) {
			m_sprite->transform.position = *m_sourcePosition;
		}
		if ( m_sourceAngle != NULL ) {
			m_sprite->transform.localRotation = Quaternion::CreateAxisAngle( Vector3d::up , *m_sourceAngle );
		}
		if ( m_sourceScale != NULL ) {
			m_sprite->transform.localScale = *m_sourceScale;
		}
	}

	// Perform animation
	m_spriteFrame += m_spriteFramespeed;
	if ( m_spriteFrame < 0 )
	{
		m_spriteFrame += m_spriteFramecount;
	}
	else if ( m_spriteFrame >= m_spriteFramecount )
	{
		m_spriteFrame -= m_spriteFramecount;
	}
}

//		PostStepSynchronus()
// Used to update the geometry generated for rendering.
void SpriteContainer::PostStepSynchronus ( void )
{
	// TODO: Make the mesh and push it to the renderer. That's right: we stream that shit.
}