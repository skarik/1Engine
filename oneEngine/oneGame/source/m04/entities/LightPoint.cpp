#include "LightPoint.h"
#include "renderer/light/RrLight.h"

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
		m_light = new RrLight();
		m_light->color = color;
		m_light->falloff_range = range;
		m_light->falloff_invpower = power;
		m_light->position = position;
		m_light->position.z = range * -0.3F;

		// Disable update on this object
		active = false;
	}
}