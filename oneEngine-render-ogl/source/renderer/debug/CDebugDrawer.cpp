
// Includes
#include "CDebugDrawer.h"
#include "core/math/Line.h"
#include "core/math/Ray.h"
#include "core/math/Color.h"

#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

using namespace Debug;

// Static Variables
CDebugDrawer* Debug::Drawer = NULL;
CDebugDrawer* CDebugDrawer::mActive = NULL;

// Constructor
CDebugDrawer::CDebugDrawer ( void )
	: CRenderableObject ()
{
	mActive = this;
	Debug::Drawer = this;

	// Set the default white material
	glMaterial* defaultMat = new glMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( TEX_DIFFUSE, new CTexture( "textures/white.jpg" ) );
	defaultMat->passinfo.push_back( glPass() );
	defaultMat->passinfo[0].shader = new glShader( "shaders/sys/fullbright.glsl" );
	defaultMat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	defaultMat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	defaultMat->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	defaultMat->passinfo[0].b_depthmask = true;
	SetMaterial( defaultMat );
	defaultMat->removeReference();
}
// Destructor
CDebugDrawer::~CDebugDrawer ( void )
{
	if ( mActive == this )
	{
		mActive = NULL;
		Debug::Drawer = NULL;
	}
}

// Rendering
bool CDebugDrawer::Render ( const char pass )
{
	// If there's no lines in the list, no need to render.
	if ( avLineList.size() < 0 )
	{
		return true;
	}

	GLd_ACCESS;

	// Bind wanted pass
	m_material->bindPass(pass);

	// Disable normal depth testing
	glDisable( GL_DEPTH_TEST );

	// Draw all the lines in the list
	GLd.BeginPrimitive( GL_LINES );
	GLd.P_PushTexcoord( 0,0 );
	GLd.P_PushNormal( 0,0,0 );
	for ( uint i = 0; i < avLineList.size(); ++i )
	{
		GLd.P_PushColor( avColorList[i] );
		GLd.P_AddVertex( avLineList[i].start );
		GLd.P_AddVertex( avLineList[i].end );
	}
	GLd.EndPrimitive();

	// Reenable normal depth testing
	glEnable( GL_DEPTH_TEST );

	// Return success
	return true;
}

bool CDebugDrawer::EndRender ( void )
{
	avLineList.clear();
	avColorList.clear();
	return true;
}

// Adding lines to the list
void CDebugDrawer::DrawLine ( const Line& newLine, const Color& color )
{
	if ( mActive != NULL )
	{
		mActive->avLineList.push_back( newLine );
		mActive->avColorList.push_back( color );
	}
}
void CDebugDrawer::DrawLine ( const Vector3d& p1, const Vector3d& p2, const Color& color )
{
	if ( mActive != NULL )
	{
		Line newLine;
		newLine.start = p1;
		newLine.end = p2;
		mActive->avLineList.push_back( newLine );
		mActive->avColorList.push_back( color );
	}
}
void CDebugDrawer::DrawRay ( const Ray& newRay, const Color& color )
{
	if ( mActive != NULL )
	{
		Line newLine;
		newLine.start = newRay.pos;
		newLine.end = newRay.pos + newRay.dir;
		mActive->avLineList.push_back( newLine );
		mActive->avColorList.push_back( color );
	}
}