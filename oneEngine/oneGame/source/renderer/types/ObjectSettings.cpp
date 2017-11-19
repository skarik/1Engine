
#include "core/math/Math.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/texture/CTexture.h"
#include "ObjectSettings.h"

renderer::_n_hint_rendering_information* renderer::m_default_hint_options;

renderer::_n_hint_rendering_information::_n_hint_rendering_information ( void )
{
	for ( uint i = 0; i < kRenderHintCOUNT; ++i ) 
	{
		mats_default[i]		= NULL;
		mats_default_skin[i]= NULL;
		mats_transparent[i]	= NULL;
		mats_transparent_skin[i]	= NULL;

		clear_type[i]	= kClearDepth;
		clear_color[i]	= Color( 0,0,0, 0.0 );
	}

	int layer;
	// Setup default replacement settings
	
	// Setup shadow shader renderer replacement. (Will replace any material with no shadow pass)
	layer = math::log2(kRenderHintShadowColor);
	mats_default[layer]	= new RrMaterial;
	mats_default[layer]->setTexture( TEX_MAIN, new CTexture("null") );
	mats_default[layer]->m_diffuse = Color( 0,0,0, 1.0f );
	mats_default[layer]->passinfo.push_back( RrPassForward() );
	mats_default[layer]->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	mats_default[layer]->passinfo[0].shader = new RrShader( "shaders/sys/black.glsl" );

	mats_default_skin[layer]	= new RrMaterial;
	mats_default_skin[layer]->m_isSkinnedShader = true;
	mats_default_skin[layer]->setTexture( TEX_MAIN, new CTexture("null") );
	mats_default_skin[layer]->m_diffuse = Color( 0,0,0, 1.0f );
	mats_default_skin[layer]->passinfo.push_back( RrPassForward() );
	mats_default_skin[layer]->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	mats_default_skin[layer]->passinfo[0].shader = new RrShader( "shaders/sys/black.glsl", renderer::SHADER_TAG_SKINNING );

	clear_type[layer] = kClearDepthAndColor;
	clear_color[layer]= Color( 0,0,0, 1.0 );

	// Setup sky glow render shader replacement. (Will replace any material with no shadow pass)
	layer = math::log2(kRenderHintSkyglow);
	mats_default[layer]	= new RrMaterial;
	mats_default[layer]->setTexture( TEX_MAIN, new CTexture("null") );
	mats_default[layer]->m_diffuse = Color( 0,0,0, 1.0f );
	mats_default[layer]->passinfo.push_back( RrPassForward() );
	mats_default[layer]->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	mats_default[layer]->passinfo[0].shader = new RrShader( "shaders/sys/black.glsl" );

	mats_default_skin[layer]	= new RrMaterial;
	mats_default_skin[layer]->m_isSkinnedShader = true;
	mats_default_skin[layer]->setTexture( TEX_MAIN, new CTexture("null") );
	mats_default_skin[layer]->m_diffuse = Color( 0,0,0, 1.0f );
	mats_default_skin[layer]->passinfo.push_back( RrPassForward() );
	mats_default_skin[layer]->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	mats_default_skin[layer]->passinfo[0].shader = new RrShader( "shaders/sys/black.glsl", renderer::SHADER_TAG_SKINNING );

	clear_type[layer] = kClearDepthAndColor;
	clear_color[layer]= Color( 0,0,0, 1.0 );
}