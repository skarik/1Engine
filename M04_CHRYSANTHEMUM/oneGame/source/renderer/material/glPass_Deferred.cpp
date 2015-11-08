
#include "glShader.h"
#include "glPass_Deferred.h"
#include "renderer/types/ObjectSettings.h"
#include <algorithm>

glPass_Deferred::glPass_Deferred ( void )
	: m_blend_mode(Renderer::BM_NONE), m_transparency_mode(Renderer::ALPHAMODE_NONE),
	m_lighting_mode(Renderer::LI_NORMAL), m_diffuse_method(Renderer::Deferred::DIFFUSE_DEFAULT),
	m_rimlight_strength(0.2f),
	m_ready(false), shader(NULL)
{

}
glPass_Deferred::~glPass_Deferred( void )
{
	if ( shader ) {
		shader->ReleaseReference();
	}
}

// Need to do a lot of file building.
/*void glPass_Deferred::buildShader ( void )
{

}*/

#include "glMaterial.h"
#include "core/system/IO/FileUtils.h"

void glMaterial::deferred_shader_build( uchar pass )
{
	glPass_Deferred& dpass = deferredinfo[pass];
	// Build a shader based on the dpass.

	string				t_shaderName;
	GLE::shader_tag_t	t_shaderTag;

	// Generate shader name based on settings
	t_shaderName = ".game/.dfshaders/shader";
	t_shaderName += ".df" + std::to_string((uint)dpass.m_diffuse_method);
	t_shaderName += (dpass.m_blend_mode==Renderer::BM_ADD) ? ".bm_a" : ( (dpass.m_blend_mode==Renderer::BM_SOFT_ADD) ? ".bm_s" : "" );
	t_shaderName += ".am" + std::to_string((uint)dpass.m_transparency_mode);
	t_shaderName += m_isSkinnedShader ? ".skinning" : "";

	// Change shader tags
	t_shaderTag = GLE::SHADER_TAG_DEFAULT;
	if ( m_isSkinnedShader )
	{
		t_shaderTag = GLE::SHADER_TAG_SKINNING;
	}

	// ============================
	// Build the vertex shader
	{
		string t_shaderNameVert = t_shaderName+".vert";
		IO::ClearFile( t_shaderNameVert.c_str() );
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "\n#version 140\n\n" );
		// Add in the vertex model inputs
		if ( !m_isSkinnedShader ) {
			if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
				IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertin.vert" );
			}
			else {
				IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertin_terrain.vert" );
			}
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertin.skinning.vert" );
		}
		IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/matxin.vert" );
		IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertex_uniforms.vert" );
		IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/sysfog.glsl" );
		if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
			IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertout.vert" );
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertout_terrain.vert" );
		}
		// Create the main routine
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "\n\nvoid main ( void ) {\n" );
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "vec4 ov_vertex;\n" );
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "vec4 ov_normal;\n" );
		if ( !m_isSkinnedShader ) {
			if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TREESYS ) {
				IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertpos.vert" );
			}
			else {
				IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertpos.systree.vert" );
			}
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertpos.skinning.vert" );
		}
		IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertex_trans.vert" );
		if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
			IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertout_main.vert" );
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), ".res/shaders/def/vertout_main_terrain.vert" );
		}
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "\n}" );
	}

	// ============================
	// Build the pixel shader
	{
		string t_shaderNameFrag = t_shaderName+".frag";
		IO::ClearFile( t_shaderNameFrag.c_str() );
		IO::AppendStringToFile( t_shaderNameFrag.c_str(), "\n#version 330\n\n" );
		// Add outputs
		IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/frag_system_outputs.frag" );
		// Add uniform input
		IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/sysfog.glsl" );
		IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/frag_surface.frag" );
		IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/frag_system_inputs.frag" );
		// Add buffer inputs
		{
			switch ( dpass.m_diffuse_method )
			{
			case Renderer::Deferred::DIFFUSE_TREESYS:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/frag_samplers_systree.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_TERRAIN:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/frag_samplers_terrain.frag" );
				break;
			default:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/frag_samplers.frag" );
				break;
			}
		}
		// Add in inputs from vertex shader
		if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
			IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/frag_vertex_inputs.frag" );
		}
		else {
			IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/frag_vertex_inputs_terrain.frag" );
		}
		// Create the color functions
		{	// Diffuse
			switch ( dpass.m_diffuse_method )
			{
			case Renderer::Deferred::DIFFUSE_SKIN:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_skin.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_EYES:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_eyes.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_SKYSPHERE:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_sky.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_ALPHA_ADDITIVE:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_1_add.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_TERRAIN:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_terrain.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_TREESYS:
				if ( dpass.m_transparency_mode == Renderer::ALPHAMODE_NONE ) {
					IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_systree.frag" );
				}
				else if ( dpass.m_transparency_mode == Renderer::ALPHAMODE_ALPHATEST ) {
					IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_0_alphatest.frag" );
				}
				break;
			case Renderer::Deferred::DIFFUSE_DEFAULT:
			default:
				if ( dpass.m_transparency_mode == Renderer::ALPHAMODE_NONE ) {
					IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_0.frag" );
				}
				else if ( dpass.m_transparency_mode == Renderer::ALPHAMODE_ALPHATEST ) {
					IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_diffuse_0_alphatest.frag" );
				}
				break;
			}
		}
		{	// Glow
			switch ( dpass.m_diffuse_method )
			{
			case Renderer::Deferred::DIFFUSE_SKIN:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_glow_skin.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_EYES:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_glow_eyes.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_SKYSPHERE:
			case Renderer::Deferred::DIFFUSE_ALPHA_ADDITIVE:
			case Renderer::Deferred::DIFFUSE_TREESYS:
			case Renderer::Deferred::DIFFUSE_HAIR:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_glow_off.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_TERRAIN:
			case Renderer::Deferred::DIFFUSE_DEFAULT:
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_glow_0.frag" );
				break;
			}
		}
		{	// Lighting
			if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_lighting_0.frag" );
			}
			else {
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/color_lighting_terrain.frag" );
			}
		}
		// Create the main routine
		IO::AppendStringToFile( t_shaderNameFrag.c_str(), "\n\nvoid main ( void ) {\n" );
		{
			if ( dpass.m_blend_mode == Renderer::ALPHAMODE_TRANSLUCENT || dpass.m_blend_mode == Renderer::BM_ADD || dpass.m_blend_mode == Renderer::BM_SOFT_ADD )
			{
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/main_transparent.frag" );
			}
			else
			{
				IO::AppendFile( t_shaderNameFrag.c_str(), ".res/shaders/def/main.frag" );
			}
		}
		IO::AppendStringToFile( t_shaderNameFrag.c_str(), "\n}" );
	}

	// Create the shader
	dpass.shader = new glShader( t_shaderName+".glsl", t_shaderTag );
}
