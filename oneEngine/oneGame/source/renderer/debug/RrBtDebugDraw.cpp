#include "RrBtDebugDraw.h"

#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"
#include "physical/interface/tobt.h"

#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/debug/RrDebugDrawer.h"
#include "gpuw/Buffers.h"
#include "gpuw/Device.h"

RrBtDebugDraw::RrBtDebugDraw ( PrWorld* associated_world )
	: btIDebugDraw(), RrRenderObject(),
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

	// Set up the default white lines
	RrPass linePass;
	linePass.m_type = kPassTypeForward;
	linePass.m_alphaMode = renderer::kAlphaModeTranslucent;
	linePass.m_cullMode = gpu::kCullModeNone;
	linePass.m_depthWrite = true;
	linePass.m_depthTest = gpu::kCompareOpAlways;
	linePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	linePass.m_primitiveType = gpu::kPrimitiveTopologyLineList;
	linePass.setTexture( TEX_MAIN, RrTexture::Load("null") );
	linePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
	linePass.m_layer = renderer::kRenderLayerV2D;
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor,
											renderer::shader::Location::kNormal};
	linePass.setVertexSpecificationByCommonList(t_vspec, 4);
	PassInitWithInput(0, &linePass);
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
{// GL_ACCESS

	if (m_haveNewUpload)
	{
		// copy over the indicies
		size_t sz_bufferIndices = sizeof(uint16_t) * m_indexData.size();
		m_buffer_indices.free(NULL);
		m_buffer_indices.initAsIndexBuffer(NULL, gpu::kIndexFormatUnsigned16, m_indexData.size());
		void* data = m_buffer_indices.map(NULL, gpu::kTransferStream);
		if (data != NULL)
		{
			memcpy(data, m_indexData.data(), sz_bufferIndices);
			m_buffer_indices.unmap(NULL);
		}

		// copy over the vertices
		m_buffer_vertPositions.free(NULL);
		m_buffer_vertPositions.initAsVertexBuffer(NULL, gpu::kFormatR32G32B32SFloat, m_vertexPositions.size());
		m_buffer_vertPositions.uploadElements(NULL, m_vertexPositions.data(), m_vertexPositions.size(), gpu::kTransferStream);
		m_buffer_vertColors.free(NULL);
		m_buffer_vertColors.initAsVertexBuffer(NULL, gpu::kFormatR32G32B32A32SFloat, m_vertexColors.size());
		m_buffer_vertColors.uploadElements(NULL, m_vertexColors.data(), m_vertexColors.size(), gpu::kTransferStream);
	}

	// Success!
	return true;
}

//		PreRender()
// Push the model's uniform up up.
bool RrBtDebugDraw::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject( XrTransform(), cameraPass );
	return true;
}

//		Render()
// Render the model using the 2D engine's style
bool RrBtDebugDraw::Render ( const rrRenderParams* params ) 
{
	// Do not render if no buffer to render with
	if (m_gpuIndexCount <= 0)
	{
		return true;
	}

	gpu::GraphicsContext* gfx = params->context_graphics;

	// For now, we will render the same way as the 3d meshes render
	//m_material->m_bufferSkeletonSize = 0;
	//m_material->m_bufferMatricesSkinning = 0;
	//m_material->bindPass(pass);

	// Bind VAO and render:
	//BindVAO( pass, m_buffer_verts, m_buffer_tris );
	//GL.DrawElements( GL_LINES, m_gpuIndexCount, GL_UNSIGNED_INT, 0 );

	gpu::Pipeline* pipeline = GetPipeline( params->pass );

	// set the pipeline
	gfx->setPipeline(pipeline);
	// bind the vertex buffers
	gfx->setVertexBuffer(renderer::shader::kVBufferSlotPosition, &m_buffer_vertPositions, 0);
	gfx->setVertexBuffer(renderer::shader::kVBufferSlotColor, &m_buffer_vertColors, 0);
	// bind the cbuffers
	gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
	// TODO:
	// bind the index buffer
	gfx->setIndexBuffer(&m_buffer_indices, gpu::kIndexFormatUnsigned16);
	// draw now
	gfx->drawIndexed(m_gpuIndexCount, 0, 0);

	// Success!
	return true;
}

void RrBtDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	// Set up new frame
	if ( m_haveNewUpload == false )
	{
		m_vertexPositions.clear();
		m_vertexColors.clear();
		m_indexData.clear();
		m_haveNewUpload = true;
	}

	// Add vertex data & index data
	/*arModelVertex vert;
	memset(vert.rawbytes, 0, sizeof(arModelVertex));
	vert.color = physical::ar(color);
	vert.color.w = 1.0F;*/

	//vert.position = physical::ar(from);
	m_indexData.push_back((uint16_t)m_vertexPositions.size());
	m_vertexPositions.push_back(physical::ar(from));
	m_vertexColors.push_back(physical::ar(color));
	//m_vertexData.push_back(vert);

	//vert.position = physical::ar(to);
	m_indexData.push_back((uint16_t)m_vertexPositions.size());
	m_vertexPositions.push_back(physical::ar(to));
	m_vertexColors.push_back(physical::ar(color));
	//m_vertexData.push_back(vert);
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