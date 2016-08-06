
#include "SpriteContainer.h"
#include "render2d/object/sprite/CStreamedRenderable2D.h"

using namespace Engine2D;


SpriteContainer::SpriteContainer ( Vector3d* position, Real* angle, Vector3d* scale )
	: CLogicObject(),
	m_sourcePosition(position), m_sourceAngle(angle), m_sourceScale(scale),
	m_sprite(NULL)
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

//		UpdateSpriteProperties()
// Pulls sprite info from the m_sprite object
void SpriteContainer::UpdateSpriteProperties ( void )
{
	if ( m_sprite != NULL )
	{
		// Pull sprite info from the sprite info stored on the renderer
		m_spriteSize = m_sprite->GetSpriteInfo().framesize;
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
	modeldata->vertices[0].x = 0;
	modeldata->vertices[0].y = 0;
	modeldata->vertices[0].z = 0;
	modeldata->vertices[0].u = 0.0F;
	modeldata->vertices[0].v = 0.0F;

	modeldata->vertices[1].x = (Real)m_spriteSize.x;
	modeldata->vertices[1].y = 0;
	modeldata->vertices[1].z = 0;
	modeldata->vertices[1].u = 1.0F;
	modeldata->vertices[1].v = 0.0F;

	modeldata->vertices[2].x = (Real)m_spriteSize.x;
	modeldata->vertices[2].y = (Real)m_spriteSize.y;
	modeldata->vertices[2].z = 0;
	modeldata->vertices[2].u = 1.0F;
	modeldata->vertices[2].v = 1.0F;

	modeldata->vertices[3].x = 0;
	modeldata->vertices[3].y = (Real)m_spriteSize.y;
	modeldata->vertices[3].z = 0;
	modeldata->vertices[3].u = 0.0F;
	modeldata->vertices[3].v = 1.0F;

	// Push resultant shit
	m_sprite->StreamLockModelData();
}