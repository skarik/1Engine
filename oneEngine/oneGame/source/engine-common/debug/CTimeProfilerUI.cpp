
#include "CTimeProfilerUI.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/object/immediate/immediate.h"

using namespace debug;

CTimeProfilerUI::CTimeProfilerUI ( void )
	: CRenderableObject()
{
	renderSettings.renderHints = kRenderHintWorld;
	renderLayer = renderer::kRLV2D;

	fntDebug	= new RrFontTexture ( "YanoneKaffeesatz-R.otf", 12, FW_BOLD );
	matFntDebug = new RrMaterial;
	matFntDebug->m_diffuse = Color( 0.2f,0.0f,0.4f );
	matFntDebug->setTexture( TEX_MAIN, fntDebug );
	matFntDebug->passinfo.push_back( RrPassForward() );
	matFntDebug->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	matFntDebug->passinfo[0].shader = new RrShader( ".res/shaders/v2d/default.glsl" );
	SetMaterial( matFntDebug );

	visible = true;
}

CTimeProfilerUI::~CTimeProfilerUI ( void )
{
	delete fntDebug;
	//matFntDebug->removeReference();
	//delete matFntDebug;
}

bool CTimeProfilerUI::Render ( const char pass )
{
	// this eentire thing needs to be rethought
	
	//GL_ACCESS GLd_ACCESS;

	if ( pass != 0 ) 
		return false;

	////GL.beginOrtho();
	//core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//GLd.DrawSet2DScaleMode();

	//Vector2d drawPos;
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