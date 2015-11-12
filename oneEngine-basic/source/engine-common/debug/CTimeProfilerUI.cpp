
#include "CTimeProfilerUI.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

using namespace Debug;

CTimeProfilerUI::CTimeProfilerUI ( void )
	: CRenderableObject()
{
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	fntDebug	= new CBitmapFont ( "YanoneKaffeesatz-R.otf", 12, FW_BOLD );
	matFntDebug = new glMaterial;
	matFntDebug->m_diffuse = Color( 0.2f,0.0f,0.4f );
	matFntDebug->setTexture( 0, fntDebug );
	matFntDebug->passinfo.push_back( glPass() );
	matFntDebug->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matFntDebug->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
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
	GL_ACCESS GLd_ACCESS;

	if ( pass != 0 ) 
		return false;

	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();

	Vector2d drawPos;
	drawPos.x =  0.02f;
	drawPos.y =  0.42f;

	//matFntDebug->bind();
	//	fntDebug->Set();
	matFntDebug->bindPass(0);
		{
			// Draw an entry for every value in the profiler list
			//boost::mutex::scoped_lock( TimeProfiler.delta_lock );
			std::lock_guard<std::mutex> lock( TimeProfiler.delta_lock );
			for ( CTimeProfiler::TimeProfileMap::iterator it = TimeProfiler.mTimeProfiles.begin(); it != TimeProfiler.mTimeProfiles.end(); ++it )
			{
				GLd.DrawAutoText( drawPos.x,drawPos.y, "%s:", it->first.c_str() );
				GLd.DrawAutoText( drawPos.x + 0.085f,drawPos.y, " %lf ms", std::min<double>(fabs(it->second.delta),4000.0) );
				drawPos.y += 0.016f;
			}
		}
	//	fntDebug->Unbind();
	//matFntDebug->unbind();

	GL.endOrtho();

	return true;
}