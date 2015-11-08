//===============================================================================================//
//	class glPass
// Forward renderer render properties storage information. Only stores pass information.
// Does not actually apply the rendering routines.
//===============================================================================================//
#ifndef _GL_PASS_H_
#define _GL_PASS_H_

#include "core/types/types.h"
#include "renderer/types/ObjectSettings.h"

class glShader;

//=========================================//
// glPass - entire storage class exported
//=========================================//
class RENDER_API glPass 
{
public:
	static	bool enabled_attributes [16];

public:
	glPass ( void );

	glPass ( const glPass& other );
	glPass ( glPass&& other );
	~glPass( void );
	glPass& operator= ( glPass& other );
	glPass& operator= ( glPass&& other );

public:
	Renderer::eAlphaMode		m_transparency_mode;
	ftype						f_alphatest_value;
	bool						b_depthmask;

	Renderer::eDrawBlendMode	m_blend_mode;
	Renderer::eDrawLightingMode	m_lighting_mode;

	Renderer::eDrawFaceMode		m_face_mode;


public:
	// Pass procedural?
	bool	m_procedural;
	// Delayed pass information
	int		m_delay;
	// Pass masking information
	uint32_t	m_hint;

	glShader*	shader;
};

#endif//_GL_PASS_H_