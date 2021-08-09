#include <algorithm>
#include "renderer/state/Settings.h"
#include "renderer/camera/RrCamera.h"
#include "RrLight.h"

namespace renderer
{
	std::vector<RrLight*> g_lightList;

	const std::vector<RrLight*>& LightList ( void )
	{
		return g_lightList;
	}
}

class RrLightComparison
{
public:
	RrCamera*			m_camera;

public:
	// Light comparison.
	// Returns true if light1 should come before light2
	bool operator() ( RrLight* light1, RrLight* light2 )
	{
		// Make sure active lights are first in the listing
		if (light1->active != light2->active)
			return light1->active;
	
		// Make sure directional lights come in first
		if (light1->type != light2->type)
			return (light1->type == kLightTypeDirectional);

		// If directional...
		if (light1->type == kLightTypeDirectional)
		{
			// Sort by the brightness:
			Real lum1 = light1->color.red * .299F + light1->color.green * .587F + light1->color.blue * .114F;
			Real lum2 = light2->color.red * .299F + light2->color.green * .587F + light2->color.blue * .114F;
			// Brighter is first.
			return lum1 > lum2;
		}
		// Otherwise...
		else
		{
			// Sort by distance:
			Vector3f delta1 = light1->position - m_camera->transform.position;
			Vector3f delta2 = light2->position - m_camera->transform.position;
			// Closer is first:
			return delta1.sqrMagnitude() < delta2.sqrMagnitude();
		}
	}
};

RrLight::RrLight ( void )
	: RrLogicObject()
{
	renderer::g_lightList.push_back( this );
}

RrLight::~RrLight ( void )
{
	const auto index = find( renderer::g_lightList.begin(), renderer::g_lightList.end(), this );
	if ( index != renderer::g_lightList.end() )
	{
		renderer::g_lightList.erase( index );
	}
	else
	{
		ARCORE_ERROR("Light was not in list.");
	}
}

void RrLight::PreStepSynchronus ( void )
{
	; // Nothing.
}

//void RrLight::UpdateLights ( RrCamera* rendering_camera )
//{
//	//// Sort the lights for the renderer
//	//RrLightComparison comparison;
//	//comparison.m_camera = rendering_camera;
//	//std::sort(lightList.begin(), lightList.end(), comparison);
//
//	//// Count active lights
//	//lightsActive = 0;
//	//for (size_t i = 0; i < lightList.size(); ++i)
//	//{
//	//	if (!lightList[i]->active)
//	//	{
//	//		lightsActive = (uint16_t)(i);
//	//	}
//	//}
//}
