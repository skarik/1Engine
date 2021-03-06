//===============================================================================================//
//	class RrPassForward
// Forward renderer render properties storage information. Only stores pass information.
// Does not actually apply the rendering routines.
//===============================================================================================//
#ifndef _GL_PASS_H_
#define _GL_PASS_H_

#include "core/types/types.h"
#include "renderer/types/ObjectSettings.h"

class RrShaderProgram;

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
	bool						m_dirty;

	renderer::eAlphaMode		m_transparency_mode;
	Real						f_alphatest_value;
	bool						b_depthmask;
	bool						b_depthtest;

	renderer::eDrawBlendMode	m_blend_mode;
	renderer::eDrawLightingMode	m_lighting_mode;

	renderer::eDrawFaceMode		m_face_mode;

public:
	
	//	set2DCommon () : Sets internal settings to common values for 2D/UI rendering.
	// Depth masking and testing disabled, no lighting, alpha blending, backface culling disabled.
	void	set2DCommon ( void );


public:
	// Pass procedural?
	bool	m_procedural;
	// Delayed pass information
	int		m_delay;
	// Pass masking information
	uint32_t	m_hint;

	RrShaderProgram*	m_program;
};

#endif//_GL_PASS_H_