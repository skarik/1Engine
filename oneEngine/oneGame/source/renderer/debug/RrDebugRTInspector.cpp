#include "RrDebugRTInspector.h"
#include "core/math/Color.h"
#include "renderer/texture/RrRenderTexture.h"
//#include "renderer/material/RrMaterial.h"

//#include "renderer/system/glMainSystem.h"
//#include "renderer/system/glDrawing.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"

#include "renderer/object/immediate/immediate.h"

using namespace debug;

// Static Variables
RrDebugRTInspector* debug::RTInspector = NULL;

// Constructor
RrDebugRTInspector::RrDebugRTInspector ( void )
	: CRenderableObject ()
{
	renderLayer = renderer::kRLV2D;
	debug::RTInspector = this;

	bDrawRTs = true;

	RrPass rtPass;
	rtPass.m_type = kPassTypeForward;
	rtPass.m_surface.diffuseColor = Color( 1.0F, 1, 1 );
	rtPass.setTexture( TEX_MAIN, RrTexture::Load("null") );
	rtPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	rtPass.utilSetupAs2D();
	PassInitWithInput(0, &rtPass);

	//// Set the default white material
	//RrMaterial* defaultMat = new RrMaterial;
	//defaultMat->passinfo.push_back( RrPassForward() );
	//defaultMat->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	//defaultMat->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	//defaultMat->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	//defaultMat->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
	//defaultMat->setTexture( TEX_DIFFUSE, new RrTexture( "textures/white.jpg" ) );
	//SetMaterial( defaultMat );
	//defaultMat->removeReference();
}
// Destructor
RrDebugRTInspector::~RrDebugRTInspector ( void )
{
	if ( debug::RTInspector == this )
	{
		debug::RTInspector = NULL;
	}
}
// Rendering
bool RrDebugRTInspector::BeginRender ( void )
{
	visible = bDrawRTs;
	return visible;
}

bool RrDebugRTInspector::PreRender ( void )
{
	m_material->prepareShaderConstants();
	return true;
}
bool RrDebugRTInspector::Render ( const char pass )
{ //GL_ACCESS;
	int currentX = 0, currentY = 0;

	for ( uint rt_i = 0; rt_i < rtList.size(); ++rt_i )
	{
		if ( rtList[rt_i]->GetHeight() )
		{
			// Bind the render texture and render it.
			// TODO: ADD A PROPERTY GETTER FOR RT. All *RELEVANT* targets need to be rendered out.
			// TODO: Properly implement this nonsense
			//if ( rtList[rt_i]->GetColorSampler() )
			//{
			//	m_material->setSampler( TEX_SLOT0, rtList[rt_i]->GetColorSampler(), GL.Enum(Texture2D) );

			//	rrMeshBuilder2D builder (4);
			//	builder.addRect(
			//		Rect( (Real)currentX, (Real)currentY, 128.0F*((Real)rtList[rt_i]->GetWidth()/(Real)rtList[rt_i]->GetHeight()),128.0F ),
			//		Color( 1.0F, 1.0F, 1.0F, 1.0F ),
			//		false);
			//	RrScopedMeshRenderer renderer;
			//	renderer.render(this, m_material, pass, builder);

			//	currentY += 128;
			//}
			//if ( rtList[rt_i]->GetDepthSampler() )
			//{
			//	m_material->setSampler( TEX_SLOT0, rtList[rt_i]->GetDepthSampler(), GL.Enum(Texture2D) );

			//	rrMeshBuilder2D builder (4);
			//	builder.addRect(
			//		Rect( (Real)currentX, (Real)currentY, 128.0F*((Real)rtList[rt_i]->GetWidth()/(Real)rtList[rt_i]->GetHeight()),128.0F ),
			//		Color( 1.0F, 1.0F, 1.0F, 1.0F ),
			//		false);
			//	RrScopedMeshRenderer renderer;
			//	renderer.render(this, m_material, pass, builder);

			//	currentY += 128;
			//}

		}
	}

	// Return success
	return true;
}

void RrDebugRTInspector::AddWatch ( RrRenderTexture* rtToWatch )
{
	std::vector<RrRenderTexture*>::iterator findResult = find( rtList.begin(), rtList.end(), rtToWatch );
	if ( findResult == rtList.end() ) {
		rtList.push_back( rtToWatch );
	}
}
void RrDebugRTInspector::RemoveWatch ( RrRenderTexture* rtToUnwatch )
{
	std::vector<RrRenderTexture*>::iterator findResult = find( rtList.begin(), rtList.end(), rtToUnwatch );
	if ( findResult != rtList.end() ) {
		rtList.erase( findResult );
	}
}