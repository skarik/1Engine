#include "core/math/Math.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/texture/RrTexture.h"
#include "ObjectSettings.h"
//
//renderer::_n_hint_rendering_information* renderer::m_default_hint_options;
//
//renderer::_n_hint_rendering_information::_n_hint_rendering_information ( void )
//{
//	for ( uint i = 0; i < kRenderHintCOUNT; ++i ) 
//	{
//		mats_default[i]		= NULL;
//		mats_default_skin[i]= NULL;
//		mats_transparent[i]	= NULL;
//		mats_transparent_skin[i]	= NULL;
//
//		clear_type[i]	= kClearDepth;
//		clear_color[i]	= Color( 0,0,0, 0.0 );
//	}
//
//	int layer;
//	// Setup default replacement settings
//	
//	// Setup shadow shader renderer replacement. (Will replace any material with no shadow pass)
//	layer = kRenderHintShadowColor;
//	mats_default[layer]	= new RrMaterial;
//	mats_default[layer]->setTexture( TEX_MAIN, core::Orphan(RrTexture::Load("null")) );
//	mats_default[layer]->m_diffuse = Color( 0, 0, 0, 1.0F );
//	mats_default[layer]->enablePassForward<0>(true);
//	mats_default[layer]->getPassForward<0>().m_lighting_mode = renderer::LI_NONE;
//	mats_default[layer]->getPassForward<0>().m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/black_vv.spv", "shaders/sys/black_p.spv"});
//
//	mats_default_skin[layer]	= new RrMaterial;
//	mats_default_skin[layer]->m_isSkinnedShader = true;
//	mats_default_skin[layer]->setTexture( TEX_MAIN, core::Orphan(RrTexture::Load("null")) );
//	mats_default_skin[layer]->m_diffuse = Color( 0,0,0, 1.0f );
//	mats_default_skin[layer]->enablePassForward<0>(true);
//	mats_default_skin[layer]->getPassForward<0>().m_lighting_mode = renderer::LI_NONE;
//	mats_default_skin[layer]->getPassForward<0>().m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/black_skinning_vv.spv", "shaders/sys/black_p.spv"});
//
//	clear_type[layer] = kClearDepthAndColor;
//	clear_color[layer]= Color( 0, 0, 0, 1.0F );
//
//	// Setup sky glow render shader replacement. (Will replace any material with no shadow pass)
//	layer = kRenderHintSkyglow;
//	mats_default[layer]	= new RrMaterial;
//	mats_default[layer]->setTexture( TEX_MAIN, core::Orphan(RrTexture::Load("null")) );
//	mats_default[layer]->m_diffuse = Color( 0,0,0, 1.0f );
//	mats_default[layer]->enablePassForward<0>(true);
//	mats_default[layer]->getPassForward<0>().m_lighting_mode = renderer::LI_NONE;
//	mats_default[layer]->getPassForward<0>().m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/black_vv.spv", "shaders/sys/black_p.spv"});
//
//	mats_default_skin[layer]	= new RrMaterial;
//	mats_default_skin[layer]->m_isSkinnedShader = true;
//	mats_default_skin[layer]->setTexture( TEX_MAIN, core::Orphan(RrTexture::Load("null")) );
//	mats_default_skin[layer]->m_diffuse = Color( 0,0,0, 1.0f );
//	mats_default_skin[layer]->enablePassForward<0>(true);
//	mats_default_skin[layer]->getPassForward<0>().m_lighting_mode = renderer::LI_NONE;
//	mats_default_skin[layer]->getPassForward<0>().m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/black_skinning_vv.spv", "shaders/sys/black_p.spv"});
//
//	clear_type[layer] = kClearDepthAndColor;
//	clear_color[layer]= Color( 0, 0, 0, 1.0F );
//}