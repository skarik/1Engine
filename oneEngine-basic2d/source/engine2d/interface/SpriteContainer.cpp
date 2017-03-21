
#include "SpriteContainer.h"
#include "render2d/object/sprite/CStreamedRenderable2D.h"

#include "core/math/Math.h"

using namespace Engine2D;


SpriteContainer::SpriteContainer ( Vector3d* position, Real* angle, Vector3d* scale )
	: CLogicObject(),
	m_sourcePosition(position), m_sourceAngle(angle), m_sourceScale(scale),
	m_sprite(NULL),
	m_requireRestream(false)
{
	m_sprite = new CStreamedRenderable2D();
}
SpriteContainer::~SpriteContainer ( void )
{
	if ( m_sprite != NULL )
	{
		delete_safe(m_sprite);
	}
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
			m_sprite->transform.position = *m_sourcePosition;
			// TODO: Remove this rounding from here
			m_sprite->transform.position.x = (Real)Math::round(m_sprite->transform.position.x);
			m_sprite->transform.position.y = (Real)Math::round(m_sprite->transform.position.y);
		}
		if ( m_sourceAngle != NULL ) {
			m_sprite->transform.rotation = Quaternion::CreateAxisAngle( Vector3d::up , *m_sourceAngle );
		}
		if ( m_sourceScale != NULL ) {
			m_sprite->transform.scale = *m_sourceScale;
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

//		GetSpriteRect()
// Pulls untransformed sprite rect, using the m_sprite object and taking into account origin.
Rect SpriteContainer::GetSpriteRect ( void )
{
	Rect rect;
	rect.pos = Vector2d( (Real)-m_spriteOrigin.x, (Real)-m_spriteOrigin.y );
	rect.size = Vector2d( (Real)m_spriteSize.x, (Real)m_spriteSize.y );
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
	CModelData* modeldata = m_sprite->GetModelData();

	// Generate the buffer information when shit is needed
	if ( modeldata->triangleNum == 0 )
	{
		modeldata->triangleNum = 2;
		modeldata->triangles = new CModelTriangle [2];

		modeldata->triangles[0].vert[0] = 0;
		modeldata->triangles[0].vert[1] = 1;
		modeldata->triangles[0].vert[2] = 2;

		modeldata->triangles[1].vert[0] = 0;
		modeldata->triangles[1].vert[1] = 2;
		modeldata->triangles[1].vert[2] = 3;
	}
	if ( modeldata->vertexNum == 0 )
	{
		modeldata->vertexNum = 4;
		modeldata->vertices = new CModelVertex [4];
		memset( modeldata->vertices, 0, sizeof(CModelVertex) * 4 );

		for ( uint i = 0; i < 4; ++i )
		{
			modeldata->vertices[i].r = 1.0F;
			modeldata->vertices[i].g = 1.0F;
			modeldata->vertices[i].b = 1.0F;
			modeldata->vertices[i].a = 1.0F;
		}
	}

	// Set the sprite information
	Rect sprite_rect = GetSpriteRect();

	modeldata->vertices[0].x = sprite_rect.pos.x;
	modeldata->vertices[0].y = sprite_rect.pos.y;
	modeldata->vertices[0].z = m_doffsetTop * sprite_rect.size.y;
	modeldata->vertices[0].u = 0.0F;
	modeldata->vertices[0].v = 0.0F;

	modeldata->vertices[1].x = sprite_rect.pos.x + sprite_rect.size.x;
	modeldata->vertices[1].y = sprite_rect.pos.y;
	modeldata->vertices[1].z = m_doffsetTop * sprite_rect.size.y;
	modeldata->vertices[1].u = 1.0F;
	modeldata->vertices[1].v = 0.0F;

	modeldata->vertices[2].x = sprite_rect.pos.x + sprite_rect.size.x;
	modeldata->vertices[2].y = sprite_rect.pos.y + sprite_rect.size.y;
	modeldata->vertices[2].z = m_doffsetBottom * sprite_rect.size.y;
	modeldata->vertices[2].u = 1.0F;
	modeldata->vertices[2].v = 1.0F;

	modeldata->vertices[3].x = sprite_rect.pos.x;
	modeldata->vertices[3].y = sprite_rect.pos.y + sprite_rect.size.y;
	modeldata->vertices[3].z = m_doffsetBottom * sprite_rect.size.y;
	modeldata->vertices[3].u = 0.0F;
	modeldata->vertices[3].v = 1.0F;

	// Push resultant shit
	m_sprite->StreamLockModelData();

	// Just streamed, no need for restream.
	m_requireRestream = false;
}