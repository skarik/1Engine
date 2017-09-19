
#include "RrShader.h"
#include "RrPassForward.h"
#include "renderer/types/ObjectSettings.h"
#include <algorithm>

bool RrPassForward::enabled_attributes [16] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};

RrPassForward::RrPassForward ( void )
	: m_dirty(true),
	m_transparency_mode(renderer::ALPHAMODE_NONE), f_alphatest_value(0.5f), b_depthmask(true),
	m_blend_mode(renderer::BM_NORMAL), m_lighting_mode(renderer::LI_NORMAL),
	m_face_mode(renderer::FM_FRONT),
	m_procedural(false),
	m_delay(0), m_hint(RL_WORLD), shader(NULL)
{

}


RrPassForward::RrPassForward ( const RrPassForward& other )
	: m_dirty(true),
	m_transparency_mode(other.m_transparency_mode), f_alphatest_value(other.f_alphatest_value), b_depthmask(other.b_depthmask),
	m_blend_mode(other.m_blend_mode), m_lighting_mode(other.m_lighting_mode),
	m_face_mode(other.m_face_mode),
	m_procedural(other.m_procedural),
	m_delay(other.m_delay), m_hint(other.m_hint), shader(other.shader)
{
	if ( shader ) {
		shader->GrabReference();
	}
}
RrPassForward::RrPassForward ( RrPassForward&& other )
	: m_dirty(true),
	m_transparency_mode(other.m_transparency_mode), f_alphatest_value(other.f_alphatest_value), b_depthmask(other.b_depthmask),
	m_blend_mode(other.m_blend_mode), m_lighting_mode(other.m_lighting_mode),
	m_face_mode(other.m_face_mode),
	m_procedural(other.m_procedural),
	m_delay(other.m_delay), m_hint(other.m_hint), shader( other.shader )
{
	other.shader = NULL;
}
RrPassForward::~RrPassForward( void )
{
	if ( shader )
	{
		shader->ReleaseReference();
	}
}
RrPassForward& RrPassForward::operator= ( RrPassForward& other )
{
	m_dirty = true;
	m_transparency_mode		= other.m_transparency_mode;
	f_alphatest_value		= other.f_alphatest_value;
	b_depthmask				= other.b_depthmask;
	m_blend_mode			= other.m_blend_mode;
	m_lighting_mode			= other.m_lighting_mode;
	m_face_mode				= other.m_face_mode;
	m_procedural			= other.m_procedural;
	m_delay					= other.m_delay;
	m_hint					= other.m_hint;

	std::swap(shader, other.shader);
    return *this;
}
RrPassForward& RrPassForward::operator= ( RrPassForward&& other )
{
	m_dirty = true;
	m_transparency_mode		= other.m_transparency_mode;
	f_alphatest_value		= other.f_alphatest_value;
	b_depthmask				= other.b_depthmask;
	m_blend_mode			= other.m_blend_mode;
	m_lighting_mode			= other.m_lighting_mode;
	m_face_mode				= other.m_face_mode;
	m_procedural			= other.m_procedural;
	m_delay					= other.m_delay;
	m_hint					= other.m_hint;

	shader = other.shader;
	other.shader = NULL;
    return *this;
}