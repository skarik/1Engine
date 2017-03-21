
#include "core/math/Math.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "ObjectSettings.h"

Renderer::_n_hint_rendering_information* Renderer::m_default_hint_options;

Renderer::_n_hint_rendering_information::_n_hint_rendering_information ( void )
{
	for ( uint i = 0; i < RL_LAYER_COUNT; ++i ) 
	{
		mats_default[i]		= NULL;
		mats_default_skin[i]= NULL;
		mats_transparent[i]	= NULL;
		mats_transparent_skin[i]	= NULL;

		clear_type[i]	= CLEAR_DEPTH;
		clear_color[i]	= Color( 0,0,0, 0.0 );
	}

	int layer;
	// Setup default replacement settings
	
	// Setup shadow shader renderer replacement. (Will replace any material with no shadow pass)
	layer = Math::log2(RL_SHADOW_COLOR);
	mats_default[layer]	= new glMaterial;
	mats_default[layer]->setTexture( TEX_MAIN, new CTexture("null") );
	mats_default[layer]->m_diffuse = Color( 0,0,0, 1.0f );
	mats_default[layer]->passinfo.push_back( glPass() );
	mats_default[layer]->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	mats_default[layer]->passinfo[0].shader = new glShader( ".res/shaders/sys/black.glsl" );

	mats_default_skin[layer]	= new glMaterial;
	mats_default_skin[layer]->m_isSkinnedShader = true;
	mats_default_skin[layer]->setTexture( TEX_MAIN, new CTexture("null") );
	mats_default_skin[layer]->m_diffuse = Color( 0,0,0, 1.0f );
	mats_default_skin[layer]->passinfo.push_back( glPass() );
	mats_default_skin[layer]->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	mats_default_skin[layer]->passinfo[0].shader = new glShader( ".res/shaders/sys/black.glsl", GLE::SHADER_TAG_SKINNING );

	clear_type[layer] = CLEAR_DEPTH_COLOR;
	clear_color[layer]= Color( 0,0,0, 1.0 );

	// Setup sky glow render shader replacement. (Will replace any material with no shadow pass)
	layer = Math::log2(RL_SKYGLOW);
	mats_default[layer]	= new glMaterial;
	mats_default[layer]->setTexture( TEX_MAIN, new CTexture("null") );
	mats_default[layer]->m_diffuse = Color( 0,0,0, 1.0f );
	mats_default[layer]->passinfo.push_back( glPass() );
	mats_default[layer]->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	mats_default[layer]->passinfo[0].shader = new glShader( ".res/shaders/sys/black.glsl" );

	mats_default_skin[layer]	= new glMaterial;
	mats_default_skin[layer]->m_isSkinnedShader = true;
	mats_default_skin[layer]->setTexture( TEX_MAIN, new CTexture("null") );
	mats_default_skin[layer]->m_diffuse = Color( 0,0,0, 1.0f );
	mats_default_skin[layer]->passinfo.push_back( glPass() );
	mats_default_skin[layer]->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	mats_default_skin[layer]->passinfo[0].shader = new glShader( ".res/shaders/sys/black.glsl", GLE::SHADER_TAG_SKINNING );

	clear_type[layer] = CLEAR_DEPTH_COLOR;
	clear_color[layer]= Color( 0,0,0, 1.0 );
}