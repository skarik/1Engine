#include "renderer/exceptions/exceptions.h"
#include "renderer/state/RrRenderer.h"
//#include "renderer/material/RrMaterial.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrTexture.h"
//#include "renderer/system/glMainSystem.h"
#include "renderer/camera/RrCamera.h"
#include "gpuw/Pipeline.h"

#include "RrRenderObject.h"

// ==Constructor
//  adds render object to the list of RO in RrRenderer
//  sets visibility to true
RrRenderObject::RrRenderObject ( void )
	/*: m_material(NULL), m_vao_info(NULL), m_vao_count(0), m_vao_maxcount(0)*/
	: m_passEnabled(), m_pipelineReady()
{
	// Set default layer mode
	renderSettings.renderHints = kRenderHintBitmaskALL;

	//InitMaterials();
	//RrRenderObject::SetMaterial( RrMaterial::Default );

	// TODO: Should renderables get a default forward pass?

	//renderLayer = renderer::kRLWorld;
	//id = RrRenderer::Active->AddRO( this );
	RrRenderer::Listings::AddToUnsorted(this, id);
	visible = true;
}

// ==Destructor

//  removes render object from the list of RO in RrRenderer
RrRenderObject::~RrRenderObject ( void )
{
	// Remove the object from the list immediately
	//RrRenderer::Active->RemoveRO( id );
	RrRenderer::Listings::GetWorld(id)->RemoveObject(this);

	// Remove material reference
	//SetMaterial( NULL );
	PassesFree();

	// Remove VAOs
	//PassinfoClear();
	//transform.RemoveReference();
	FreePipelines();
}

void RrRenderObject::AddToWorld ( RrWorld* world )
{
	ARCORE_ASSERT_MSG(world->world_index != UINT32_MAX, "Invalid world. Was the world added to a renderer first?");
	ARCORE_ASSERT(id.world_index == rrId::kWorldInvalid);
	RrRenderer::Listings::RemoveFromUnsorted(this, id);
	id = world->AddObject(this);
}

//===============================================================================================//
// Pass management
//===============================================================================================//

void RrRenderObject::PushCbufferPerObject ( const XrTransform& worldTransform, const rrCameraPass* cameraPass )
{
	// Update matrix constants
	{
		Matrix4x4 modelTRS, modelR;
		core::TransformUtility::TRSToMatrix4x4(worldTransform, modelTRS, modelR);

		modelTRS = !modelTRS;
		modelR  = !modelR;

		renderer::cbuffer::rrPerObjectMatrices matrices;
		matrices.modelTRS = modelTRS;
		matrices.modelR  = modelR;
		matrices.modelViewProjection = (cameraPass != NULL) ? (modelTRS * cameraPass->m_viewprojTransform) : (modelTRS);
		matrices.modelViewProjectionInverse = matrices.modelViewProjection.inverse();

		// TODO: Create a "m_isStatic" flag for if MVP should be calculated in the vertex shader.

		// TODO: Create the buffer & push it
		if (!m_cbufPerObjectMatrices.valid())
			m_cbufPerObjectMatrices.initAsConstantBuffer(NULL, sizeof(matrices));
		m_cbufPerObjectMatrices.upload(NULL, &matrices, sizeof(matrices), gpu::TransferStyle::kTransferStream);
	}

	// Update surface constants
	{
		renderer::cbuffer::rrPerObjectSurface surface [kPass_MaxPassCount];
		
		for (int i = 0; i < kPass_MaxPassCount; ++i)
		{
			// If pass is enabled & the pass surface needs sync:
			if (m_passEnabled[i] && !m_passSurfaceSynced[i])
			{
				if (!m_cbufPerObjectSurfaces[i].valid())
					m_cbufPerObjectSurfaces[i].initAsConstantBuffer(NULL, sizeof(m_passes[i].m_surface));
				m_cbufPerObjectSurfaces[i].upload(NULL, &m_passes[i].m_surface, sizeof(m_passes[i].m_surface), gpu::TransferStyle::kTransferStream);

				m_passSurfaceSynced[i] = true; // Mark as synced so we don't upload again.
			}
		}
	}
}

//	PassInitWithInput(pass, passData) : Sets up a new pass on the given slot.
// Creates a copy of passData without changing reference counts.
void RrRenderObject::PassInitWithInput ( int pass, RrPass* passData )
{
	ARCORE_ASSERT(pass >= 0 || pass < kPass_MaxPassCount);

	// Ensure vertex data exists
	ARCORE_ASSERT_MSG(passData->m_vertexSpecification != NULL, "Cannot initialize a pass without setting the vertex specification.");

	// Free up the given pass
	PassFree(pass);
	
	// Copy the pass over
	m_passes[pass].assignFrom(*passData);
	m_passEnabled[pass] = true;
	m_passSurfaceSynced[pass] = false;

	// Verify the pass is valid
	ARCORE_ASSERT(m_passes[pass].validate());
}

void RrRenderObject::PassFree ( int pass )
{
	ARCORE_ASSERT(pass >= 0 || pass < kPass_MaxPassCount);

	// Remove the previous pass, and decrement references then.
	if (m_passEnabled[pass])
	{
		if (m_passes[pass].m_program != NULL)
		{
			m_passes[pass].m_program->RemoveReference();
			m_passes[pass].m_program = NULL;
		}

		for (int i = 0; i < kPass_MaxTextureSlots; ++i)
		{
			m_passes[pass].setTexture((rrTextureSlot)(TEX_SLOT0 + i), (RrTexture*)NULL);
		}
	}

	// Disable the pass now
	m_passEnabled[pass] = false;
}

renderer::cbuffer::rrPerObjectSurface& RrRenderObject::PassGetSurface ( int pass )
{
	ARCORE_ASSERT(pass >= 0 || pass < kPass_MaxPassCount);
	m_passSurfaceSynced[pass] = false;
	return m_passes[pass].m_surface;
}

RrPass::SafeAccessor RrRenderObject::PassAccess ( int pass )
{
	ARCORE_ASSERT(pass >= 0 || pass < kPass_MaxPassCount);
	return RrPass::SafeAccessor(&m_passes[pass]);
}

void RrRenderObject::PassesFree ( void )
{
	for (int i = 0; i < kPass_MaxPassCount; ++i)
	{
		if (m_passEnabled[i]) {
			PassFree(i);
		}
	}
}

//===============================================================================================//
// Pipeline management
//===============================================================================================//

gpu::Pipeline* RrRenderObject::GetPipeline ( const uchar pass, bool* bindings_active_at_locations )
{
	if (m_pipelineReady[pass])
	{
		// check for changes

		m_pipelines[pass].destroy(NULL);
		m_pipelineReady[pass] = false;
	}

	if (!m_pipelineReady[pass])
	{
		RrPass* rp = &m_passes[pass];

		gpu::PipelineCreationDescription pipeline_desc = {};
		pipeline_desc.ia_primitiveRestartEnable = true;
		pipeline_desc.ia_topology = rp->m_primitiveType;

		pipeline_desc.shader_pipeline = &rp->m_program->GetShaderPipeline();

		// TODO: proper handling of pipelines & topology. this may need to be handled at runtime

		// set up the vertex attributes:
		gpu::VertexInputBindingDescription binding_desc [16];
		gpu::VertexInputAttributeDescription attrib_desc [16];

		pipeline_desc.vv_inputBindingsCount = rp->m_vertexSpecificationCount;
		pipeline_desc.vv_inputBindings = binding_desc;
		pipeline_desc.vv_inputAttributesCount = rp->m_vertexSpecificationCount;
		pipeline_desc.vv_inputAttributes = attrib_desc;
	
		for (int i = 0; i < rp->m_vertexSpecificationCount; ++i)
		{
			// set up the binding
			binding_desc[i].binding = rp->m_vertexSpecification[i].binding;
			binding_desc[i].inputRate = (gpu::InputRate)rp->m_vertexSpecification[i].dataInputRate;
			binding_desc[i].stride = rp->m_vertexSpecification[i].dataStride;

			// set up the attribs
			attrib_desc[i].binding = rp->m_vertexSpecification[i].binding;
			attrib_desc[i].location = (uint32_t)rp->m_vertexSpecification[i].location;
			attrib_desc[i].format = rp->m_vertexSpecification[i].dataFormat;
			attrib_desc[i].offset = rp->m_vertexSpecification[i].dataOffset;
		}

		// Modify the pipelines based on the bindings active
		if (bindings_active_at_locations)
		{
			for (int i = 0; i < rp->m_vertexSpecificationCount; ++i)
			{
				if (!bindings_active_at_locations[(int)rp->m_vertexSpecification[i].location])
				{
					binding_desc[i].stride = 0; // Force zero stride for the given position
				}
			}
		}

		// Create pipeline now
		m_pipelines[pass].create(NULL, &pipeline_desc);

		// Mark pipeline as ready
		m_pipelineReady[pass] = true;
	}
	else
	{
		// check for changes with the current state.

		// pipeline should be a dummy object for current state?
		// renderer::pipeline should be an on-demand created object as another layer?
	}

	return &m_pipelines[pass];
}

void RrRenderObject::FreePipelines ( void )
{
	for (int i = 0; i < kPass_MaxPassCount; ++i)
	{
		if (m_pipelineReady[i] || m_pipelines[i].valid())
		{
			m_pipelines[i].destroy(NULL);
			m_pipelineReady[i] = false;
		}
	}
}


//===============================================================================================//
// Render Status
//===============================================================================================//
float RrRenderObject::GetRenderDistance ( void )
{
	return renderDistance;
}