
// Includes
#include "CDebugRTInspector.h"
#include "core/math/Color.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/material/glMaterial.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

using namespace Debug;

// Static Variables
CDebugRTInspector* Debug::RTInspector = NULL;

// Constructor
CDebugRTInspector::CDebugRTInspector ( void )
	: CRenderableObject ()
{
	renderType = Renderer::V2D;
	Debug::RTInspector = this;

	bDrawRTs = true;

	// Set the default white material
	defaultMat = new glMaterial;
	defaultMat->passinfo.push_back( glPass() );
	defaultMat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	defaultMat->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	defaultMat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	defaultMat->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
	defaultMat->setTexture( 0, new CTexture( "textures/white.jpg" ) );
	defaultMat->removeReference();
	SetMaterial( defaultMat );
}
// Destructor
CDebugRTInspector::~CDebugRTInspector ( void )
{
	if ( Debug::RTInspector == this )
	{
		Debug::RTInspector = NULL;
	}
	//delete defaultMat;
}
// Rendering
bool CDebugRTInspector::BeginRender ( void )
{
	visible = bDrawRTs;
	return visible;
}

bool CDebugRTInspector::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS

	defaultMat->m_diffuse = Color(1,1,1,1);
	
	GL.beginOrtho();

	GLd.DrawSet2DScaleMode();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );

	int currentX = 0, currentY = 0;

	//for ( vector<CRenderTexture*>::iterator rt_it = rtList.begin(), rt_it != rtList.end(); ++rt_it )
	for ( uint rt_i = 0; rt_i < rtList.size(); ++rt_i )
	{
		if ( rtList[rt_i]->GetHeight() )
		{
			// Bind the render texture and render it.
			// TODO: ADD A PROPERTY GETTER FOR RT. All *RELEVANT* targets need to be rendered out.
			//rtList[rt_i]->Bind();
			//glColor4f( 1,1,1,1.0f );

			if ( rtList[rt_i]->GetColorSampler() )
			{
				defaultMat->setSampler( 0, rtList[rt_i]->GetColorSampler(), GL.Enum(Texture2D) );
				defaultMat->bindPass(0);
				GLd.DrawRectangle( currentX, currentY, 128.0F*((Real)rtList[rt_i]->GetWidth()/(Real)rtList[rt_i]->GetHeight()),128.0F );
				currentY += 128;
			}
			if ( rtList[rt_i]->GetDepthSampler() )
			{
				defaultMat->setSampler( 0, rtList[rt_i]->GetDepthSampler(), GL.Enum(Texture2D) );
				defaultMat->bindPass(0);
				GLd.DrawRectangle( currentX, currentY, 128.0F*((Real)rtList[rt_i]->GetWidth()/(Real)rtList[rt_i]->GetHeight()),128.0F );
				currentY += 128;
			}

		}
	}

	GL.endOrtho();

	// Return success
	return true;
}

void CDebugRTInspector::AddWatch ( CRenderTexture* rtToWatch )
{
	std::vector<CRenderTexture*>::iterator findResult = find( rtList.begin(), rtList.end(), rtToWatch );
	if ( findResult == rtList.end() ) {
		rtList.push_back( rtToWatch );
	}
}
void CDebugRTInspector::RemoveWatch ( CRenderTexture* rtToUnwatch )
{
	std::vector<CRenderTexture*>::iterator findResult = find( rtList.begin(), rtList.end(), rtToUnwatch );
	if ( findResult != rtList.end() ) {
		rtList.erase( findResult );
	}
}