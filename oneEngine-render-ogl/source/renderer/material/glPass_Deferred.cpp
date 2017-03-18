
#include "glShader.h"
#include "glPass_Deferred.h"
#include "renderer/types/ObjectSettings.h"
#include <algorithm>

glPass_Deferred::glPass_Deferred ( void )
	: m_blend_mode(Renderer::BM_NONE), m_transparency_mode(Renderer::ALPHAMODE_NONE),
	/*m_lighting_mode(Renderer::LI_NORMAL), m_diffuse_method(Renderer::Deferred::DIFFUSE_DEFAULT),*/
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
#include "core-ext/system/io/Resources.h"
#include <cctype>
#include <map>

void glMaterial::deferred_shader_build( uchar pass )
{
	glPass_Deferred& dpass = deferredinfo[pass];
	// Build a shader based on the dpass.

	string				t_shaderName;
	GLE::shader_tag_t	t_shaderTag;

	// Generate shader name based on settings
	t_shaderName = ".game/.dfshaders/shader";
	//t_shaderName += ".df" + std::to_string((uint)dpass.m_diffuse_method);
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
		string shader = IO::ReadFileToString( (Core::Resources::GetPrimaryResourcePath() + "shaders/def_alt/surface_default.vert").c_str() );
		size_t pos;

		// Remove macro at the top
		pos = shader.find("#define VERTEX_PROCESSOR");
		if (pos != string::npos)
		{
			shader.erase(pos, sizeof("#define VERTEX_PROCESSOR"));
		}

		// Add macro at the bottom
		pos = shader.find("VERTEX_PROCESSOR");
		if (pos != string::npos)
		{
			shader.erase(pos, sizeof("VERTEX_PROCESSOR"));
			string stub;
			if ( t_shaderTag == GLE::SHADER_TAG_SKINNING )
				stub = IO::ReadFileToString( (Core::Resources::GetPrimaryResourcePath() + "shaders/def_alt/vertex_proccessing.skinning.vert.stub").c_str() );
			else if ( t_shaderTag == GLE::SHADER_TAG_DEFAULT )
				stub = IO::ReadFileToString( (Core::Resources::GetPrimaryResourcePath() + "shaders/def_alt/vertex_proccessing.vert.stub").c_str() );
			shader.insert(pos, stub);
		}

		string t_shaderNameVert = t_shaderName + ".vert";
		IO::ClearFile( t_shaderNameVert.c_str() );
		IO::AppendStringToFile( t_shaderNameVert.c_str(), shader.c_str() );
	}

	// ============================
	// Build the pixel shader
	{
		string shader = IO::ReadFileToString( (Core::Resources::GetPrimaryResourcePath() + "shaders/def_alt/surface_default.frag").c_str() );

		string t_shaderNameFrag = t_shaderName+".frag";
		IO::ClearFile( t_shaderNameFrag.c_str() );
		IO::AppendStringToFile( t_shaderNameFrag.c_str(), shader.c_str() );
	}

	// ============================
	// Build the vertex shader
	/*{
		arstring<256> prefix ( (Core::Resources::GetPrimaryResourcePath() + "shaders/def/").c_str() );
		string t_shaderNameVert = t_shaderName+".vert";
		IO::ClearFile( t_shaderNameVert.c_str() );
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "\n#version 140\n\n" );
		// Add in the vertex model inputs
		if ( !m_isSkinnedShader ) {
			if ( !m_isInstancedShader ) {
				if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
					IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertin.vert" );
				}
				else {
					IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertin_terrain.vert" );
				}
			}
			else {
				IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertin.instanced.vert" );
			}
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertin.skinning.vert" );
		}
		IO::AppendFile( t_shaderNameVert.c_str(), prefix+"matxin.vert" );
		IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertex_uniforms.vert" );
		IO::AppendFile( t_shaderNameVert.c_str(), prefix+"sysfog.glsl" );
		if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
			IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertout.vert" );
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertout_terrain.vert" );
		}
		// Create the main routine
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "\n\nvoid main ( void ) {\n" );
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "vec4 ov_vertex;\n" );
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "vec4 ov_normal;\n" );
		if ( !m_isSkinnedShader ) {
			if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TREESYS ) {
				IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertpos.vert" );
			}
			else {
				IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertpos.systree.vert" );
			}
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertpos.skinning.vert" );
		}
		if ( !m_isInstancedShader ) {
			IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertex_trans.vert" );
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertex_trans.instanced.vert" );
		}
		if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
			IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertout_main.vert" );
		}
		else {
			IO::AppendFile( t_shaderNameVert.c_str(), prefix+"vertout_main_terrain.vert" );
		}
		IO::AppendStringToFile( t_shaderNameVert.c_str(), "\n}" );
	}

	// ============================
	// Build the pixel shader
	{
		arstring<256> prefix ( (Core::Resources::GetPrimaryResourcePath() + "shaders/def/").c_str() );
		string t_shaderNameFrag = t_shaderName+".frag";
		IO::ClearFile( t_shaderNameFrag.c_str() );
		IO::AppendStringToFile( t_shaderNameFrag.c_str(), "\n#version 330\n\n" );
		// Add outputs
		IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"frag_system_outputs.frag" );
		// Add uniform input
		IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"sysfog.glsl" );
		IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"frag_surface.frag" );
		IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"frag_system_inputs.frag" );
		// Add buffer inputs
		{
			switch ( dpass.m_diffuse_method )
			{
			case Renderer::Deferred::DIFFUSE_TREESYS:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"frag_samplers_systree.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_TERRAIN:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"frag_samplers_terrain.frag" );
				break;
			default:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"frag_samplers.frag" );
				break;
			}
		}
		// Add in inputs from vertex shader
		if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
			IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"frag_vertex_inputs.frag" );
		}
		else {
			IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"frag_vertex_inputs_terrain.frag" );
		}
		// Create the color functions
		{	// Diffuse
			switch ( dpass.m_diffuse_method )
			{
			case Renderer::Deferred::DIFFUSE_SKIN:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_skin.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_EYES:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_eyes.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_SKYSPHERE:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_sky.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_ALPHA_ADDITIVE:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_1_add.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_TERRAIN:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_terrain.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_TREESYS:
				if ( dpass.m_transparency_mode == Renderer::ALPHAMODE_NONE ) {
					IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_systree.frag" );
				}
				else if ( dpass.m_transparency_mode == Renderer::ALPHAMODE_ALPHATEST ) {
					IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_0_alphatest.frag" );
				}
				break;
			case Renderer::Deferred::DIFFUSE_DEFAULT:
			default:
				if ( dpass.m_transparency_mode == Renderer::ALPHAMODE_NONE ) {
					IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_0.frag" );
				}
				else if ( dpass.m_transparency_mode == Renderer::ALPHAMODE_ALPHATEST ) {
					IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_diffuse_0_alphatest.frag" );
				}
				break;
			}
		}
		{	// Glow
			switch ( dpass.m_diffuse_method )
			{
			case Renderer::Deferred::DIFFUSE_SKIN:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_glow_skin.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_EYES:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_glow_eyes.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_SKYSPHERE:
			case Renderer::Deferred::DIFFUSE_ALPHA_ADDITIVE:
			case Renderer::Deferred::DIFFUSE_TREESYS:
			case Renderer::Deferred::DIFFUSE_HAIR:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_glow_off.frag" );
				break;
			case Renderer::Deferred::DIFFUSE_TERRAIN:
			case Renderer::Deferred::DIFFUSE_DEFAULT:
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_glow_0.frag" );
				break;
			}
		}
		{	// Lighting
			if ( dpass.m_diffuse_method != Renderer::Deferred::DIFFUSE_TERRAIN ) {
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_lighting_0.frag" );
			}
			else {
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"color_lighting_terrain.frag" );
			}
		}
		// Create the main routine
		IO::AppendStringToFile( t_shaderNameFrag.c_str(), "\n\nvoid main ( void ) {\n" );
		{
			if ( dpass.m_blend_mode == Renderer::ALPHAMODE_TRANSLUCENT || dpass.m_blend_mode == Renderer::BM_ADD || dpass.m_blend_mode == Renderer::BM_SOFT_ADD )
			{
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"main_transparent.frag" );
			}
			else
			{
				IO::AppendFile( t_shaderNameFrag.c_str(), prefix+"main.frag" );
			}
		}
		IO::AppendStringToFile( t_shaderNameFrag.c_str(), "\n}" );
	}*/

	// Create the shader
	dpass.shader = new glShader( t_shaderName+".glsl", t_shaderTag );
}
