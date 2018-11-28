#include "renderer/material/RrPass.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrShaderProgram.h"

RrPass::RrPass ( void )
	: m_program(NULL), m_textures(), m_texturesRaw()
{
	utilSetupAsDefault();
}

//	utilSetupAsDefault () : Sets proprties of the pass to what the default 3D pass usually is.
// In detail: depth write on, depth test, face culling, transparency off, world layer
void RrPass::utilSetupAsDefault ( void )
{
	m_type = kPassTypeDeferred;
	m_orderOffset = 0;
	m_surface = renderer::cbuffer::rrPerObjectSurface();
	m_cullMode = gpu::kCullModeBack;
	m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	m_alphaMode = renderer::kAlphaModeNone;
	m_depthWrite = true;
	m_depthTest = gpu::kCompareOpLess;
	m_layer = renderer::kRenderLayerWorld;
}

//	utilSetupAs2D () : Sets properties of the pass to what all 2D passes typically use.
// In detail: depth write off, depth test always, no face culling, transparency on, lighting hinted off.
void RrPass::utilSetupAs2D ( void )
{
	//m_lighting_mode = renderer::LI_NONE;
	m_alphaMode = renderer::kAlphaModeTranslucent;
	m_cullMode = gpu::kCullModeNone;
	m_depthWrite = false;
	m_depthTest = gpu::kCompareOpAlways;
}

// Destructor for the pass attempts to release references to all objects in use.
// This should only include Rr Textures & Shader programs.
RrPass::~RrPass ( void )
{
	for (size_t i = 0; i < kPass_MaxTextureSlots; ++i)
	{
		if (m_textures[i] != NULL)
		{
			m_textures[i]->RemoveReference();
		}
	}
	if (m_program != NULL)
	{
		m_program->RemoveReference();
	}
}