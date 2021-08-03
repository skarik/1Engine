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
	linePass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	linePass.m_overrideDepth = true;
	linePass.m_overrideDepthWrite = true;
	linePass.m_overrideDepthTest = gpu::kCompareOpAlways;
	linePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	linePass.setTexture( TEX_MAIN, RrTexture::Load(renderer::kTextureWhite) );
	linePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/debug_lines_vv.spv", "shaders/sys/debug_lines_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor,
											renderer::shader::Location::kNormal};
	linePass.setVertexSpecificationByCommonList(t_vspec, 4);
	PassInitWithInput(0, &linePass);
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
		uint16_t iIndex		= i * 6;
		uint16_t iVertex	= i * 4;

		Vector4f lineColor	= (Vector4f)avColorList[i];
		Vector3f lineStart	= avLineList[i].start;
		Vector3f lineEnd	= avLineList[i].end;
		Vector3f lineDelta	= lineStart - lineEnd;

		// Add triangle data
		m_modeldata.indices[iIndex + 0] = iVertex + 0;
		m_modeldata.indices[iIndex + 1] = iVertex + 1;
		m_modeldata.indices[iIndex + 2] = iVertex + 2;
		m_modeldata.indices[iIndex + 3] = iVertex + 1;
		m_modeldata.indices[iIndex + 4] = iVertex + 2;
		m_modeldata.indices[iIndex + 5] = iVertex + 3;

		// Set line color and world-space offset
		m_modeldata.color[iVertex + 0] = lineColor;
		m_modeldata.color[iVertex + 1] = lineColor;
		m_modeldata.color[iVertex + 2] = lineColor;
		m_modeldata.color[iVertex + 3] = lineColor;
		m_modeldata.normal[iVertex + 0] = lineDelta;
		m_modeldata.normal[iVertex + 1] = lineDelta;
		m_modeldata.normal[iVertex + 2] = lineDelta;
		m_modeldata.normal[iVertex + 3] = lineDelta;

		// Line Start
		m_modeldata.position[iVertex + 0] = lineStart;
		m_modeldata.position[iVertex + 1] = lineStart;
		m_modeldata.texcoord0[iVertex + 0] = Vector2f(1.0F, 0.0F);
		m_modeldata.texcoord0[iVertex + 1] = Vector2f(-1.0F, 0.0F);

		// Line Ending
		m_modeldata.position[iVertex + 2] = lineEnd;
		m_modeldata.position[iVertex + 3] = lineEnd;
		m_modeldata.texcoord0[iVertex + 2] = Vector2f(1.0F, 1.0F);
		m_modeldata.texcoord0[iVertex + 3] = Vector2f(-1.0F, 1.0F);

	} // Offsets are handled within the vertex shader.

	// Update model
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