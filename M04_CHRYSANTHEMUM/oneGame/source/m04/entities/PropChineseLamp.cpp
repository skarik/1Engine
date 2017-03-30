
#include "PropChineseLamp.h"
#include "renderer/light/CLight.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(prop_chinese_lamp,M04::PropChineseLamp);

PropChineseLamp::PropChineseLamp ( void )
	: CGameBehavior(), Engine2D::SpriteContainer(&position, &m_angle, NULL)
{
	m_light = NULL;
	m_angle = 0;
}

PropChineseLamp::~PropChineseLamp ( void )
{
	delete_safe(m_light);
}

void PropChineseLamp::Update ( void )
{
	if (m_light == NULL)
	{
		m_light = new CLight();
		m_light->diffuseColor = color;
		m_light->hdrColor = color;
		m_light->range = 64.0F;
		m_light->falloff = 1.0F;
		m_light->transform.position = position;
	}
}