#include "CDebugDrawer.h"
#include "core/math/Line.h"
#include "core/math/Ray.h"
#include "core/math/Color.h"

#include "renderer/material/RrMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

using namespace debug;

CDebugDrawer* debug::Drawer			= NULL;
CDebugDrawer* CDebugDrawer::mActive	= NULL;

CDebugDrawer::CDebugDrawer ( void )
	: CStreamedRenderable3D ()
{
	mActive = this;
	debug::Drawer = this;

	// Reserve data for the lines
	m_vertices.reserve(1024);
	m_tris.reserve(512);

	// Reset model data
	memset( &m_modeldata, 0, sizeof(arModelData) );

	// Set the default white material
	RrMaterial* defaultMat = new RrMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( TEX_DIFFUSE, new CTexture( "textures/white.jpg" ) );
	defaultMat->passinfo.push_back( RrPassForward() );
	defaultMat->passinfo[0].shader = new RrShader( "shaders/sys/debug_lines.glsl" );
	defaultMat->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	defaultMat->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	defaultMat->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	defaultMat->passinfo[0].b_depthmask = true;
	SetMaterial( defaultMat );
	defaultMat->removeReference();
}
CDebugDrawer::~CDebugDrawer ( void )
{
	if ( mActive == this )
	{
		mActive = NULL;
		debug::Drawer = NULL;
	}
}

bool CDebugDrawer::PreRender ( void )
{
	// Clear the old mesh data
	m_vertices.clear();
	m_tris.clear();

	arModelVertex vert;
	memset(&vert, 0, sizeof(arModelVertex));
	arModelTriangle tri;

	for ( uint i = 0; i < avLineList.size(); ++i )
	{
		// Add triangle data
		tri.vert[0] = m_vertices.size() + 0;
		tri.vert[1] = m_vertices.size() + 1;
		tri.vert[2] = m_vertices.size() + 2;
		m_tris.push_back(tri);
		tri.vert[0] = m_vertices.size() + 0;
		tri.vert[1] = m_vertices.size() + 2;
		tri.vert[2] = m_vertices.size() + 3;
		m_tris.push_back(tri);

		// Set line color and world-space offset
		vert.color = Vector4f(avColorList[i].raw);
		vert.normal = avLineList[i].start - avLineList[i].end;

		// Line Start
		vert.position = avLineList[i].start;

		vert.texcoord0 = Vector2f(1.0F, 0.0F);
		m_vertices.push_back(vert);
		vert.texcoord0 = Vector2f(-1.0F, 0.0F);
		m_vertices.push_back(vert);

		// Line Ending
		vert.position = avLineList[i].end;

		vert.texcoord0 = Vector2f(1.0F, 0.0F);
		m_vertices.push_back(vert);
		vert.texcoord0 = Vector2f(-1.0F, 0.0F);
		m_vertices.push_back(vert);
	} // Offsets are handled within the vertex shader.

	// Update model
	m_modeldata.triangleNum	= (uint16_t)m_tris.size();
	m_modeldata.triangles	= m_tris.data();
	m_modeldata.vertexNum	= (uint16_t)m_vertices.size();
	m_modeldata.vertices	= m_vertices.data();

	// Clear line data now that we're done with it.
	avLineList.clear();
	avColorList.clear();

	// Now with the mesh built, push it to the modeldata
	StreamLockModelData();

	// Push at the end
	return CStreamedRenderable3D::PreRender();
}

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