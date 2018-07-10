#include "RrBtDebugDraw.h"

#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"
#include "physical/interface/tobt.h"

#include "renderer/material/RrMaterial.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/debug/CDebugDrawer.h"


RrBtDebugDraw::RrBtDebugDraw ( PrWorld* associated_world )
	: btIDebugDraw(), CRenderableObject(),
	m_debugMode(DBG_DrawWireframe | DBG_DrawAabb),
	m_gpuIndexCount(0), m_haveNewUpload(false)
{
	// Set the world we want to render to.
	m_world = associated_world;
	if (m_world == NULL)
	{
		m_world = PrPhysics::Active()->CurrentWorld();
	}

	// Set up the debug drawer
	if (m_world != NULL)
	{
		m_world->ApiWorld()->setDebugDrawer(this);
	}

	// Set the layer to the final overlay layer
	this->renderType = renderer::kRLV2D;

	// Create a forward pass material (see CDebugDrawer for reference)
	RrMaterial* defaultMat = new RrMaterial;
	defaultMat->m_diffuse = Color( 1,1,1,1 );
	defaultMat->setTexture( TEX_DIFFUSE, new RrTexture( "textures/white.jpg" ) );
	defaultMat->passinfo.push_back( RrPassForward() );
	defaultMat->passinfo[0].shader = new RrShader( "shaders/sys/fullbright.glsl" );
	defaultMat->passinfo[0].set2DCommon();
	defaultMat->passinfo[0].b_depthmask = true;
	defaultMat->passinfo[0].b_depthtest = false;
	SetMaterial( defaultMat );
	defaultMat->removeReference();
}

RrBtDebugDraw::~RrBtDebugDraw ( void )
{
	if (m_world)
	{
		m_world->ApiWorld()->setDebugDrawer(NULL);
	}
}


//		PostRender()
// Push the current model information to the GPU.
bool RrBtDebugDraw::EndRender ( void ) 
{ GL_ACCESS

	if (m_haveNewUpload)
	{
		// Make sure we don't destroy an existing VAO with the GL_ELEMENT_ARRAY_BUFFER binding.
		GL.BindVertexArray( 0 ); 

		// Create new buffers
		if ( m_buffer_verts == NIL )
			GL.CreateBuffer( &m_buffer_verts );
		if ( m_buffer_tris == NIL )
			GL.CreateBuffer( &m_buffer_tris );

		// Bind to some buffer objects
		GL.BindBuffer( GL_ARRAY_BUFFER,			m_buffer_verts ); // for vertex coordinates
		GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER,	m_buffer_tris ); // for face vertex indexes

		// Copy data to the buffer
		GL.UploadBuffer( GL_ARRAY_BUFFER,
			sizeof(arModelVertex) * m_vertexData.size(),
			m_vertexData.data(),
			GL_STREAM_DRAW );
		GL.UploadBuffer( GL_ELEMENT_ARRAY_BUFFER,
			sizeof(uint32_t) * m_indexData.size(),
			m_indexData.data(),
			GL_STREAM_DRAW );

		// bind with 0, so, switch back to normal pointer operation
		GL.UnbindBuffer( GL_ARRAY_BUFFER );
		GL.UnbindBuffer( GL_ELEMENT_ARRAY_BUFFER );

		// update gpu index count
		m_gpuIndexCount = m_indexData.size();

		// no new upload
		m_haveNewUpload = false;
	}

	// Success!
	return true;
}

//		PreRender()
// Push the model's uniform up up.
bool RrBtDebugDraw::PreRender ( void )
{
	//m_material->prepareShaderConstants( transform.world );
	m_material->prepareShaderConstants();
	return true;
}

//		Render()
// Render the model using the 2D engine's style
bool RrBtDebugDraw::Render ( const char pass ) 
{ GL_ACCESS
	// Do not render if no buffer to render with
	if ( m_buffer_verts == 0 || m_buffer_tris == 0 || m_gpuIndexCount == 0 )
	{
		return true;
	}

	// For now, we will render the same way as the 3d meshes render
	m_material->m_bufferSkeletonSize = 0;
	m_material->m_bufferMatricesSkinning = 0;
	m_material->bindPass(pass);

	// Bind VAO and render:
	BindVAO( pass, m_buffer_verts, m_buffer_tris );
	GL.DrawElements( GL_LINES, m_gpuIndexCount, GL_UNSIGNED_INT, 0 );

	// Success!
	return true;
}

void RrBtDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	// Set up new frame
	if ( m_haveNewUpload == false )
	{
		m_vertexData.clear();
		m_indexData.clear();
		m_haveNewUpload = true;
	}

	// Add vertex data & index data
	arModelVertex vert;
	memset(vert.rawbytes, 0, sizeof(arModelVertex));
	vert.color = physical::ar(color);
	vert.color.w = 1.0F;

	vert.position = physical::ar(from);
	m_indexData.push_back(m_vertexData.size());
	m_vertexData.push_back(vert);

	vert.position = physical::ar(to);
	m_indexData.push_back(m_vertexData.size());
	m_vertexData.push_back(vert);
}

void RrBtDebugDraw::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
	drawLine(PointOnB,PointOnB+normalOnB*distance,color);
	btVector3 ncolor(0, 0, 0);
	drawLine(PointOnB, PointOnB + normalOnB*0.01F, ncolor);
}

void RrBtDebugDraw::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

int RrBtDebugDraw::getDebugMode() const
{
	return m_debugMode;
}