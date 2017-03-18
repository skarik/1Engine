//===============================================================================================//
//	class glPass
// Concurrent renderer render properties storage information. Only stores pass information.
// Does not actually apply the rendering routines.
//===============================================================================================//
#ifndef _GL_PASS_DEFERRED_H_
#define _GL_PASS_DEFERRED_H_

#include "core/types/types.h"
#include "renderer/types/ObjectSettings.h"

class glShader;
class glMaterial;

//=========================================//
// glPass_Deferred - entire storage class exported
//=========================================//
class RENDER_API glPass_Deferred
{
public:
	glPass_Deferred ( void );
	~glPass_Deferred( void );

	Renderer::eAlphaMode		m_transparency_mode;
	Renderer::eDrawBlendMode	m_blend_mode;
	//Renderer::eDrawLightingMode	m_lighting_mode;

	//Renderer::Deferred::eDiffuseMethod	m_diffuse_method;

	float		m_rimlight_strength;

	glShader* GetShader ( void ) {  return shader; };
protected:
	friend glMaterial;

	glShader*	shader;
	bool		m_ready;

	//void buildShader ( void );
};

#endif//_GL_PASS_DEFERRED_H_