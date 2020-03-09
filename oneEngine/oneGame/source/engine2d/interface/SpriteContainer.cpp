
#include "SpriteContainer.h"
#include "render2d/object/sprite/CEditableRenderable2D.h"

#include "core/math/Math.h"

using namespace Engine2D;


SpriteContainer::SpriteContainer ( Vector3f* position, Real* angle, Vector3f* scale )
	: RrLogicObject(),
	m_sourcePosition(position), m_sourceAngle(angle), m_sourceScale(scale),
	m_sprite(NULL),
	m_requireRestream(false)
{
	m_sprite = new CEditableRenderable2D();
}
SpriteContainer::~SpriteContainer ( void )
{
	delete_safe_array(m_sprite->GetModelData()->indices);
	delete_safe_array(m_sprite->GetModelData()->position);
	delete_safe_array(m_sprite->GetModelData()->color);
	delete_safe_array(m_sprite->GetModelData()->texcoord0);
	delete_safe(m_sprite);
}

//		SetupDepthOffset
// Sets up depth offset of the sprite. Input is percent of sprite size.
// Changes values written to z-buffer, used for 3d effects.
void SpriteContainer::SetupDepthOffset ( const Real top_offset, const Real bottom_offset )
{
	m_doffsetTop	= top_offset;
	m_doffsetBottom	= bottom_offset;
	m_requireRestream = true;
}


void SpriteContainer::PreStep ( void )
{
	// Update sprite position
	if ( m_sprite != NULL )
	{
		if ( m_sourcePosition != NULL ) {
			m_sprite->transform.world.position = *m_sourcePosition;
			// TODO: Remove this rounding from here
			m_sprite->transform.world.position.x = (Real)math::round(m_sprite->transform.world.position.x);
			m_sprite->transform.world.position.y = (Real)math::round(m_sprite->transform.world.position.y);
		}
		if ( m_sourceAngle != NULL ) {
			m_sprite->transform.world.rotation = Quaternion::CreateAxisAngle( Vector3f::up , *m_sourceAngle );
		}
		if ( m_sourceScale != NULL ) {
			m_sprite->transform.world.scale = *m_sourceScale;
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

//		UpdateSpriteProperties()
// Pulls sprite info from the m_sprite object
void SpriteContainer::UpdateSpriteProperties ( void )
{
	if ( m_sprite != NULL )
	{
		// Pull sprite info from the sprite info stored on the renderer
		if ( m_spriteSize != m_sprite->GetSpriteInfo().framesize )
		{
			m_spriteSize = m_sprite->GetSpriteInfo().framesize;
			m_requireRestream = true;
		}
	}
}

//		SetSpriteSize( Vector2i size )
// Overrides the rect the sprite will use to render
void SpriteContainer::SetSpriteSize ( const Vector2i size )
{
	if ( m_spriteSize != size )
	{
		m_spriteSize = size;
		m_requireRestream = true;
	}
}

//		GetSpriteRect()
// Pulls untransformed sprite rect, using the m_sprite object and taking into account origin.
Rect SpriteContainer::GetSpriteRect ( void )
{
	Rect rect;
	rect.pos = Vector2f( (Real)-m_spriteOrigin.x, (Real)-m_spriteOrigin.y );
	rect.size = Vector2f( (Real)m_spriteSize.x, (Real)m_spriteSize.y );
	return rect;
}

//		PostStepSynchronus()
// Used to update the geometry generated for rendering.
void SpriteContainer::PostStepSynchronus ( void )
{
	if ( m_sprite == NULL || m_spriteSize.x == 0 || m_spriteSize.y == 0 )
	{
		// Don't do anything. Instead, ask for an update
		UpdateSpriteProperties();
		return;
	}

	// Also don't do anything if restreaming is off
	if ( m_requireRestream == false )
	{
		return;
	}

	// TODO: Make the mesh and push it to the renderer. That's right: we stream that shit.
	arModelData* modeldata = m_sprite->GetModelData();

	// Generate the buffer information when shit is needed
	if ( modeldata->indexNum == 0 )
	{
		modeldata->indexNum = 4;
		modeldata->indices = new uint16_t [4];

		modeldata->indices[0] = 0;
		modeldata->indices[1] = 1;
		modeldata->indices[2] = 2;
		modeldata->indices[3] = 3;
	}
	if ( modeldata->vertexNum == 0 )
	{
		modeldata->vertexNum = 4;
		modeldata->position = new Vector3f [4];
		modeldata->color = new Vector4f [4];
		modeldata->texcoord0 = new Vector3f [4];

		for ( uint i = 0; i < 4; ++i )
		{
			modeldata->color[i] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
		}
	}

	// Set the sprite information
	Rect sprite_rect = GetSpriteRect();

	modeldata->position[0] = sprite_rect.pos;
	modeldata->position[0].z = m_doffsetTop * sprite_rect.size.y;
	modeldata->texcoord0[0] = Vector2f(0.0F, 0.0F);

	modeldata->position[1] = Vector2f(sprite_rect.pos.x + sprite_rect.size.x, sprite_rect.pos.y);
	modeldata->position[1].z = m_doffsetTop * sprite_rect.size.y;
	modeldata->texcoord0[1] = Vector2f(1.0F, 0.0F);

	modeldata->position[2] = sprite_rect.pos + sprite_rect.size;
	modeldata->position[2].z = m_doffsetBottom * sprite_rect.size.y;
	modeldata->texcoord0[2] = Vector2f(1.0F, 1.0F);

	modeldata->position[3] = Vector2f(sprite_rect.pos.x, sprite_rect.pos.y + sprite_rect.size.y);
	modeldata->position[3].z = m_doffsetBottom * sprite_rect.size.y;
	modeldata->texcoord0[3] = Vector2f(0.0F, 1.0F);

	// Push resultant shit
	m_sprite->StreamLockModelData();

	// Just streamed, no need for restream.
	m_requireRestream = false;
}