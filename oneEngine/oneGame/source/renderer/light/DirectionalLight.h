
#ifndef __DIRECTIONAL_LIGHT_H_
#define __DIRECTIONAL_LIGHT_H_

// Includes
#include "CLight.h"

// Class definition
class DirectionalLight : public CLight
{
public:
	RENDER_API DirectionalLight ( void );

	//void Update ( void );
	//void LateUpdate ( void );
	RENDER_API void PreStepSynchronus ( void ) override;
	RENDER_API void UpdateLight ( void );
	RENDER_API Color GetSendVector ( void );

protected:
	float shadowRange;

	// Virtual function for specific shadow camera code
	void UpdateShadowCamera ( void );
};

#endif