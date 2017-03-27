//===============================================================================================//
//	class RrPassForward
// Concurrent renderer render properties storage information. Only stores pass information.
// Does not actually apply the rendering routines.
//===============================================================================================//
#ifndef _GL_PASS_DEFERRED_H_
#define _GL_PASS_DEFERRED_H_

#include "core/types/types.h"
#include "renderer/types/ObjectSettings.h"

class RrShader;
class RrMaterial;

//=========================================//
// RrPassDeferred - entire storage class exported
//=========================================//
class RENDER_API RrPassDeferred
{
public:
	RrPassDeferred ( void );
	~RrPassDeferred( void );

	renderer::eAlphaMode		m_transparency_mode;
	renderer::eDrawBlendMode	m_blend_mode;
	//renderer::eDrawLightingMode	m_lighting_mode;

	//renderer::Deferred::eDiffuseMethod	m_diffuse_method;

	float		m_rimlight_strength;

	RrShader* GetShader ( void ) {  return shader; };
protected:
	friend RrMaterial;

	RrShader*	shader;
	bool		m_ready;

	//void buildShader ( void );
};

#endif//_GL_PASS_DEFERRED_H_