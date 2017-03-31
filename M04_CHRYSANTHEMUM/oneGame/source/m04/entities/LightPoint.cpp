
#include "LightPoint.h"
#include "renderer/light/CLight.h"

using namespace M04;

DECLARE_OBJECT_REGISTRAR(light_point,M04::LightPoint);

LightPoint::LightPoint ( void )
	: CGameBehavior()
{
	m_light = NULL;
}

LightPoint::~LightPoint ( void )
{
	delete_safe(m_light);
}

void LightPoint::Update ( void )
{
	if (m_light == NULL)
	{
		m_light = new CLight();
		m_light->diffuseColor = color;
		m_light->hdrColor = color;
		m_light->range = range;
		m_light->falloff = power;
		m_light->position = position;

		// Disable update on this object
		active = false;
	}
}