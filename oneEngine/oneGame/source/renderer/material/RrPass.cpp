#include "renderer/material/RrPass.h"

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