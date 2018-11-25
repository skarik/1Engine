#include "RrDebugDrawer.h"
#include "core/math/Line.h"
#include "core/math/Ray.h"
#include "core/math/Color.h"
#include "core/debug/console.h"

#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrTexture.h"

using namespace debug;

RrDebugDrawer* debug::Drawer			= NULL;
RrDebugDrawer* RrDebugDrawer::mActive	= NULL;

RrDebugDrawer::RrDebugDrawer ( void )
	: CStreamedRenderable3D ()
{
	mActive = this;
	debug::Drawer = this;

	// Reserve data for the lines
	//m_vertices.reserve(1024);
	//m_tris.reserve(512);

	// Reset model data
	memset( &m_modeldata, 0, sizeof(arModelData) );
	m_modeldata.indexNum = 0;
	m_modeldata.vertexNum = 0;
	m_indexNumAlloc = 0;
	m_vertexNumAlloc = 0;

	// Set up the default white lines
	RrPass linePass;
	linePass.m_type = kPassTypeForward;
	linePass.m_alphaMode = renderer::kAlphaModeTranslucent;
	linePass.m_cullMode = gpu::kCullModeNone;
	linePass.m_depthWrite = true;
	linePass.m_depthTest = gpu::kCompareOpAlways;
	linePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	linePass.setTexture( TEX_MAIN, RrTexture::Load("null") );
	linePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/debug_lines_vv.spv", "shaders/sys/debug_lines_p.spv"}) );
	PassInitWithInput(0, &linePass);

	// Set the default white material
	//RrMaterial* defaultMat = new RrMaterial;
	//defaultMat->m_diffuse = Color( 1,1,1,1 );
	//defaultMat->setTexture( TEX_DIFFUSE, new RrTexture( "textures/white.jpg" ) );
	//defaultMat->passinfo.push_back( RrPassForward() );
	//defaultMat->passinfo[0].shader = new RrShader( "shaders/sys/debug_lines.glsl" );
	//defaultMat->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	//defaultMat->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	//defaultMat->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;
	//defaultMat->passinfo[0].b_depthmask = true;
	//SetMaterial( defaultMat );
	//defaultMat->removeReference();
}
RrDebugDrawer::~RrDebugDrawer ( void )
{
	if ( mActive == this )
	{
		mActive = NULL;
		debug::Drawer = NULL;
	}
}

bool RrDebugDrawer::PreRender ( rrCameraPass* cameraPass )
{
	// Make sure line size is smol enough.
	if (avLineList.size() >= ((1 << 16) / 6))
	{
		// Show an error in the console...
		debug::Console->PrintError("RrDebugDrawer::avLineList is too large to draw.\n");
		// Clear the list
		avLineList.clear();
		avColorList.clear();
		return true; // Exit early
	}

	// Allocate the needed data
	uint16_t l_requestedIndexCount  = (uint16_t)(avLineList.size() * 6);
	uint16_t l_requestedVertexCount = (uint16_t)(avLineList.size() * 4);
	if (m_indexNumAlloc < l_requestedIndexCount)
	{
		delete_safe_array(m_modeldata.indices);
		m_modeldata.indices = new uint16_t [l_requestedIndexCount];
	}
	if (m_vertexNumAlloc < l_requestedVertexCount)
	{
		delete_safe_array(m_modeldata.position);
		delete_safe_array(m_modeldata.texcoord0);
		delete_safe_array(m_modeldata.normal);
		delete_safe_array(m_modeldata.color);

		m_modeldata.position	= new Vector3f [l_requestedVertexCount];
		m_modeldata.texcoord0	= new Vector3f [l_requestedVertexCount];
		m_modeldata.normal		= new Vector3f [l_requestedVertexCount];
		m_modeldata.color		= new Vector4f [l_requestedVertexCount];
	}

	// Build the mesh:
	for ( uint16_t i = 0; i < avLineList.size(); ++i )
	{
		uint16_t iIndex = i * 6;
		uint16_t iVertex = i * 4;

		// Add triangle data
		m_modeldata.indices[iIndex + 0] = iVertex + 0;
		m_modeldata.indices[iIndex + 1] = iVertex + 1;
		m_modeldata.indices[iIndex + 2] = iVertex + 2;
		m_modeldata.indices[iIndex + 3] = iVertex + 0;
		m_modeldata.indices[iIndex + 4] = iVertex + 2;
		m_modeldata.indices[iIndex + 5] = iVertex + 3;

		//// Add triangle data
		//tri.vert[0] = m_vertices.size() + 0;
		//tri.vert[1] = m_vertices.size() + 1;
		//tri.vert[2] = m_vertices.size() + 2;
		//m_tris.push_back(tri);
		//tri.vert[0] = m_vertices.size() + 0;
		//tri.vert[1] = m_vertices.size() + 2;
		//tri.vert[2] = m_vertices.size() + 3;
		//m_tris.push_back(tri);

		//// Set line color and world-space offset
		//vert.color = Vector4f(avColorList[i].raw);
		//vert.normal = avLineList[i].start - avLineList[i].end;

		// Set line color and world-space offset
		m_modeldata.color[iVertex + 0] = (Vector4f)avColorList[i];
		m_modeldata.color[iVertex + 1] = (Vector4f)avColorList[i];
		m_modeldata.color[iVertex + 2] = (Vector4f)avColorList[i];
		m_modeldata.color[iVertex + 3] = (Vector4f)avColorList[i];
		m_modeldata.normal[iVertex + 0] = avLineList[i].start - avLineList[i].end;
		m_modeldata.normal[iVertex + 1] = avLineList[i].start - avLineList[i].end;
		m_modeldata.normal[iVertex + 2] = avLineList[i].start - avLineList[i].end;
		m_modeldata.normal[iVertex + 3] = avLineList[i].start - avLineList[i].end;

		// Line Start
		//vert.position = avLineList[i].start;

		//vert.texcoord0 = Vector2f(1.0F, 0.0F);
		//m_vertices.push_back(vert);
		//vert.texcoord0 = Vector2f(-1.0F, 0.0F);
		//m_vertices.push_back(vert);

		m_modeldata.position[iVertex + 0] = avLineList[i].start;
		m_modeldata.position[iVertex + 1] = avLineList[i].start;
		m_modeldata.texcoord0[iVertex + 0] = Vector2f(1.0F, 0.0F);
		m_modeldata.texcoord0[iVertex + 1] = Vector2f(-1.0F, 0.0F);

		// Line Ending
		//vert.position = avLineList[i].end;

		//vert.texcoord0 = Vector2f(1.0F, 0.0F);
		//m_vertices.push_back(vert);
		//vert.texcoord0 = Vector2f(-1.0F, 0.0F);
		//m_vertices.push_back(vert);

		m_modeldata.position[iVertex + 2] = avLineList[i].end;
		m_modeldata.position[iVertex + 3] = avLineList[i].end;
		m_modeldata.texcoord0[iVertex + 2] = Vector2f(1.0F, 0.0F);
		m_modeldata.texcoord0[iVertex + 3] = Vector2f(-1.0F, 0.0F);

	} // Offsets are handled within the vertex shader.

	// Update model
	/*m_modeldata.triangleNum	= (uint16_t)m_tris.size();
	m_modeldata.triangles	= m_tris.data();
	m_modeldata.vertexNum	= (uint16_t)m_vertices.size();
	m_modeldata.vertices	= m_vertices.data();*/
	m_modeldata.indexNum = l_requestedIndexCount;
	m_modeldata.vertexNum = l_requestedVertexCount;

	// Clear line data now that we're done with it.
	avLineList.clear();
	avColorList.clear();

	// Now with the mesh built, push it to the modeldata
	StreamLockModelData();

	// Push at the end
	return CStreamedRenderable3D::PreRender(cameraPass);
}

void RrDebugDrawer::DrawLine ( const Line& newLine, const Color& color )
{
	if ( mActive != NULL )
	{
		mActive->avLineList.push_back( newLine );
		mActive->avColorList.push_back( color );
	}
}
void RrDebugDrawer::DrawLine ( const Vector3f& p1, const Vector3f& p2, const Color& color )
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
void RrDebugDrawer::DrawRay ( const Ray& newRay, const Color& color )
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