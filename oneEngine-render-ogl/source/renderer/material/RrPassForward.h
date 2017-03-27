//===============================================================================================//
//	class RrPassForward
// Forward renderer render properties storage information. Only stores pass information.
// Does not actually apply the rendering routines.
//===============================================================================================//
#ifndef _GL_PASS_H_
#define _GL_PASS_H_

#include "core/types/types.h"
#include "renderer/types/ObjectSettings.h"

class RrShader;

//=========================================//
// RrPassForward - entire storage class exported
//=========================================//
class RENDER_API RrPassForward 
{
public:
	static	bool enabled_attributes [16];

public:
	RrPassForward ( void );

	RrPassForward ( const RrPassForward& other );
	RrPassForward ( RrPassForward&& other );
	~RrPassForward( void );
	RrPassForward& operator= ( RrPassForward& other );
	RrPassForward& operator= ( RrPassForward&& other );

public:
	renderer::eAlphaMode		m_transparency_mode;
	ftype						f_alphatest_value;
	bool						b_depthmask;

	renderer::eDrawBlendMode	m_blend_mode;
	renderer::eDrawLightingMode	m_lighting_mode;

	renderer::eDrawFaceMode		m_face_mode;


public:
	// Pass procedural?
	bool	m_procedural;
	// Delayed pass information
	int		m_delay;
	// Pass masking information
	uint32_t	m_hint;

	RrShader*	shader;
};

#endif//_GL_PASS_H_