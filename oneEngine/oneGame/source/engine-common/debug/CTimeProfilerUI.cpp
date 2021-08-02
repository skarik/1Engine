#include "CTimeProfilerUI.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrFontTexture.h"

using namespace debug;

CTimeProfilerUI::CTimeProfilerUI ( void )
	: RrRenderObject()
{
	fntDebug = RrFontTexture::Load( "YanoneKaffeesatz-R.otf", 12, FW_BOLD );
	fntDebug->AddReference();
	RrPass textPass;
	textPass.m_layer = renderer::kRenderLayerV2D;
	textPass.m_type = kPassTypeForward;
	textPass.m_surface.diffuseColor = Color( 0.2F, 0.0F, 0.4F );
	textPass.setTexture( TEX_MAIN, fntDebug );
	textPass.utilSetupAs2D();
	textPass.m_alphaMode = renderer::kAlphaModeTranslucent;
	textPass.m_program = RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"});
	PassInitWithInput(0, &textPass);

	visible = true;
}

CTimeProfilerUI::~CTimeProfilerUI ( void )
{
	//delete fntDebug;
	fntDebug->RemoveReference();
	//matFntDebug->removeReference();
	//delete matFntDebug;
}

bool CTimeProfilerUI::Render ( const rrRenderParams* params )
{
	// this eentire thing needs to be rethought, since it was very slow
	
	//GL_ACCESS GLd_ACCESS;

	if ( params->pass != 0 ) 
		return false;

	////GL.beginOrtho();
	//core::math::Cubic::FromPosition( Vector3f(0, 0, -45.0F), Vector3f((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//GLd.DrawSet2DScaleMode();

	//Vector2f drawPos;
	//drawPos.x =  0.02f;
	//drawPos.y =  0.42f;

	////matFntDebug->bind();
	////	fntDebug->Set();
	//matFntDebug->bindPass(0);
	//	{
	//		// Draw an entry for every value in the profiler list
	//		//boost::mutex::scoped_lock( TimeProfiler.delta_lock );
	//		std::lock_guard<std::mutex> lock( TimeProfiler.delta_lock );
	//		for ( CTimeProfiler::TimeProfileMap::iterator it = TimeProfiler.mTimeProfiles.begin(); it != TimeProfiler.mTimeProfiles.end(); ++it )
	//		{
	//			GLd.DrawAutoText( drawPos.x,drawPos.y, "%s:", it->first.c_str() );
	//			GLd.DrawAutoText( drawPos.x + 0.085f,drawPos.y, " %lf ms", std::min<double>(fabs(it->second.delta),4000.0) );
	//			drawPos.y += 0.016f;
	//		}
	//	}
	////	fntDebug->Unbind();
	////matFntDebug->unbind();

	////GL.endOrtho();

	return true;
}