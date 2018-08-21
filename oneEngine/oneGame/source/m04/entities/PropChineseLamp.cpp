
#include "PropChineseLamp.h"
#include "renderer/light/RrLight.h"

#include "render2d/object/sprite/CEditableRenderable2D.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(prop_chinese_lamp,M04::PropChineseLamp);

PropChineseLamp::PropChineseLamp ( void )
	: CGameBehavior(), Engine2D::SpriteContainer(&position, &m_angle, NULL)
{
	m_light = NULL;
	m_angle = 0;

	SetupDepthOffset( -1.0F, 0.0F );
	m_sprite->SpriteGenParams().normal_default = Vector3d(0, 2.0F, 1.0F).normal();
	m_sprite->SetSpriteFile("sprites/props/chinese_lamp.gal");
	m_spriteOrigin = Vector2i( m_sprite->GetSpriteInfo().fullsize.x / 2, 0 );
}

PropChineseLamp::~PropChineseLamp ( void )
{
	delete_safe(m_light);
}

void PropChineseLamp::Update ( void )
{
	// Set Z:
	position.z = -height;

	if (m_light == NULL)
	{
		m_light = new RrLight();
		m_light->diffuseColor = color;
		m_light->hdrColor = color;
		m_light->range = 256.0F;
		m_light->falloff = 2.0F;
		m_light->position = position;
		m_light->position.z *= 0.5F;
		m_light->position.y += height * 0.5F;
	}
}