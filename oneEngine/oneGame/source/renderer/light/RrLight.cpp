#include <algorithm>
#include "renderer/state/Settings.h"
#include "renderer/camera/RrCamera.h"
#include "RrLight.h"

// Usings
using std::vector;
using std::find;

// Static vars
vector<RrLight*>		RrLight::lightList;
uint16_t				RrLight::lightsActive = 0;

// Light comparison.
// Returns true if light1 should come before light2
bool RrLightComparison::operator() ( RrLight* light1, RrLight* light2 )
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

// == Constructors ==
RrLight::RrLight ( void )
	: RrLogicObject()
{
	lightList.push_back( this );

	type			= kLightTypeArea;
	area_type		= kLightAreaTypePoint;

	position		= Vector3f(0, 0, 0);
	direction		= Vector3f(0, 0, -1.0F);
	size			= Vector3f(0.1F, 0.1F, 0.1F);

	color			= Color(1.0F, 1.0F, 1.0F, 1.0F);
	falloff_range	= 500.0F;
	falloff_invpower	= 2.0F;
	falloff_passthru	= 0.0F;

	shadows_enabled	= false;
	shadows_resolution_factor	= 1.0F;

	effect_halo_strength	= 0.0F;
}

// Destructors
RrLight::~RrLight ( void )
{
	vector<RrLight*>::iterator index;
	index = find( lightList.begin(), lightList.end(), this );
	if ( index != lightList.end() )
	{
		lightList.erase( index );
	}
}

// Update
void RrLight::PreStepSynchronus ( void )
{
	; // Nothing.
}


// Update light list on render
void RrLight::UpdateLights ( RrCamera* rendering_camera )
{
	// Sort the lights for the renderer
	RrLightComparison comparison;
	comparison.m_camera = rendering_camera;
	std::sort(lightList.begin(), lightList.end(), comparison);

	// Count active lights
	lightsActive = 0;
	for (size_t i = 0; i < lightList.size(); ++i)
	{
		if (!lightList[i]->active)
		{
			lightsActive = (uint16_t)(i);
		}
	}
}
