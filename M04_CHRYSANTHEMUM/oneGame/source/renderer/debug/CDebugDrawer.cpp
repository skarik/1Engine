
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
	renderType = Renderer::World;
	mActive = this;
	Debug::Drawer = this;

	// Set the default white material
	defaultMat = new glMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( 0, new CTexture( ".res/textures/white.jpg" ) );
	defaultMat->passinfo.push_back( glPass() );
	defaultMat->passinfo[0].shader = new glShader( ".res/shaders/sys/fullbright.glsl" );
	defaultMat->removeReference();
	//defaultMat->releaseOwnership();
	//vMaterials.push_back( defaultMat );
	SetMaterial( defaultMat );
}
// Destructor
CDebugDrawer::~CDebugDrawer ( void )
{
	if ( mActive == this )
	{
		mActive = NULL;
		Debug::Drawer = NULL;
	}
	//delete defaultMat;
}

// Rendering
bool CDebugDrawer::Render ( const char pass )
{
	if ( avLineList.size() < 0 ) {
		return true;
	}
	GLd_ACCESS;

	defaultMat->bindPass(0);

	//glDepthMask( GL_TRUE );
	glDepthFunc( GL_ALWAYS );
	//glDisable( GL_LIGHTING );
	// Load identity matrix
	//glLoadIdentity();

	//glColor4f( 1.0f,1.0f,0.0f, 1.0f );

	// Draw all the lines in the list
	//std::vector<Line>::iterator index;
	GLd.BeginPrimitive( GL_LINES );
	//glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, Color( 1,1,1,1 ).start_point() );
	//for ( index = avLineList.begin(); index != avLineList.end(); index++ )
	for ( uint i = 0; i < avLineList.size(); ++i )
	{
		GLd.P_PushColor( avColorList[i] );
		//glVertex3fv( &((*index).start.x) );
		//glVertex3fv( &((*index).end.x) );
		GLd.P_AddVertex( avLineList[i].start );
		GLd.P_AddVertex( avLineList[i].end );
	}
	GLd.EndPrimitive();

	// Empty the list
	//while ( !avLineList.empty() )
	//	avLineList.pop_back();
		//avLineList.erase( avLineList.begin() );

	glDepthFunc( GL_LEQUAL );
	//glEnable( GL_LIGHTING );
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
		//pActive->avLineList.( newLine );
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