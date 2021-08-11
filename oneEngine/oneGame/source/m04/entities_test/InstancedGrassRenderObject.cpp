#include "InstancedGrassRenderObject.h"

#include "gpuw/Device.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/Material.h"
#include "renderer/material/RrShaderProgram.h"

#include "renderer/meshmodel/Loader.h"
#include "renderer/state/RrRenderer.h"

InstancedGrassRenderObject::InstancedGrassRenderObject ( void )
	: RrRenderObject()
{
	// Use a default material
	RrPass pass;
	pass.utilSetupAsDefault();
	pass.m_type = kPassTypeDeferred;
	pass.m_alphaMode = renderer::kAlphaModeAlphatest;
	pass.m_cullMode = gpu::kCullModeNone;
	pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/deferred_env/foliage_hapgrass0_vv.spv", "shaders/deferred_env/simple_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor,
											renderer::shader::Location::kNormal,
											renderer::shader::Location::kTangent,
											renderer::shader::Location::kBinormal};
	pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
	pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	pass.setTexture(rrTextureSlot::TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite));
	gpu::SamplerCreationDescription pointFilter;
	pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
	pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
	pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);

	PassInitWithInput(0, &pass);
}

void InstancedGrassRenderObject::LoadGrassMeshes ( const char* const* mesh_resources, const int mesh_count )
{
	for (int mesh_index = 0; mesh_index < mesh_count; ++mesh_index)
	{
		const char* mesh_resource = mesh_resources[mesh_index];

		// Load in mesh group:
		grassMeshType mesh_info;
		mesh_info.m_meshGroup = renderer::LoadMeshGroup(rrModelLoadParams{mesh_resource});
		mesh_info.m_meshBuffer = mesh_info.m_meshGroup->m_meshes[0];

		ARCORE_ASSERT(mesh_info.m_meshBuffer->m_modeldata->vertexNum > 0);
		ARCORE_ASSERT(mesh_info.m_meshBuffer->m_modeldata->indexNum > 0);

		mesh_types.push_back(mesh_info);
	}
}

InstancedGrassRenderObject::~InstancedGrassRenderObject ( void )
{ 
	//m_meshBuffer.FreeMeshBuffers();
	for (auto& mesh_type : mesh_types)
	{
		mesh_type.m_meshGroup->RemoveReference();
	}
}

bool InstancedGrassRenderObject::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(XrTransform(), cameraPass);

	// Resize & reset counters
	instancing_subdraw_info.resize(mesh_types.size());
	for (grassSubDrawInfo& subdraw_info : instancing_subdraw_info)
	{
		subdraw_info = grassSubDrawInfo();
	}

	// Loop through the grass and add them to the instancing arrays
	instancing_info.instance_count = 0;
	for (int32 grassVariation = 0; grassVariation < mesh_types.size(); ++grassVariation)
	{
		// Save current offset for this variation
		instancing_subdraw_info[grassVariation].base_offset = instancing_info.instance_count;

		// Acculmulate all the variations
		for (int32 grassIndex = 0; grassIndex < m_grassInfo.size(); ++grassIndex)
		{
			if (m_grassInfo[grassIndex].variation_index != grassVariation)
				continue;

			// Ensure enough room before we access instance info
			if (instancing_info.transforms.size() <= instancing_info.instance_count
				|| instancing_info.variations.size() <= instancing_info.instance_count)
			{
				instancing_info.transforms.resize(std::max<size_t>(instancing_info.instance_count, instancing_info.transforms.size() * 2) + 1);
				instancing_info.variations.resize(std::max<size_t>(instancing_info.instance_count, instancing_info.variations.size() * 2) + 1);
			}

			instancing_info.transforms[instancing_info.instance_count].transform = !m_grassInfo[grassIndex].transform; // Shaders need the matrices transposed.
			instancing_info.variations[instancing_info.instance_count].color = m_grassInfo[grassIndex].color;
			instancing_info.variations[instancing_info.instance_count].index = grassIndex;

			// Count the new grass
			instancing_info.instance_count += 1;

			// Count the specific new grass variation
			instancing_subdraw_info[grassVariation].instance_count += 1;
		}
	}

	ARCORE_ASSERT(instancing_info.instance_count <= instancing_info.variations.size());
	ARCORE_ASSERT(instancing_info.instance_count <= instancing_info.transforms.size());

	instancing_info.buffer_transforms.initAsStructuredBuffer(NULL, sizeof(grInstancedDataGrassTransform) * instancing_info.instance_count);
	instancing_info.buffer_transforms.upload(NULL, instancing_info.transforms.data(), sizeof(grInstancedDataGrassTransform) * instancing_info.instance_count, gpu::kTransferStream);

	instancing_info.buffer_variations.initAsStructuredBuffer(NULL, sizeof(grInstancedDataGrassVariation) * instancing_info.instance_count);
	instancing_info.buffer_variations.upload(NULL, instancing_info.variations.data(), sizeof(grInstancedDataGrassVariation) * instancing_info.instance_count, gpu::kTransferStream);

	// Update the offsets in the buffer

	return true;
}

bool InstancedGrassRenderObject::EndRender ( void )
{
	instancing_info.buffer_transforms.free(NULL);
	instancing_info.buffer_variations.free(NULL);

	return true;
}

//		Render()
// Render the model using the 2D engine's style
bool InstancedGrassRenderObject::Render ( const rrRenderParams* params )
{ 
	// otherwise we will render the same way 3d meshes render
	{
		if ( mesh_types.empty() && instancing_info.instance_count )
			return true; // Only render when have a valid mesh and rendering enabled

		for (int mesh_type = 0; mesh_type < mesh_types.size(); ++mesh_type)
		{
			rrMeshBuffer* meshBuffer = mesh_types[mesh_type].m_meshBuffer;
			const grassSubDrawInfo& subdrawInfo = instancing_subdraw_info[mesh_type];

			gpu::GraphicsContext* gfx = params->context_graphics;

			gpu::Buffer offset_info;
			offset_info.initAsConstantBuffer(NULL, sizeof(int32));
			offset_info.upload(gfx, (void*)&subdrawInfo.base_offset, sizeof(int32), gpu::kTransferStream);

			gpu::Pipeline* pipeline = GetPipeline( params->pass, meshBuffer->m_bufferEnabled );
			gfx->setPipeline(pipeline);
			// Set up the material helper...
			renderer::Material(this, gfx, params, pipeline)
				// set the depth & blend state registers
				.setDepthStencilState()
				.setRasterizerState()
				// bind the samplers & textures
				.setBlendState()
				.setTextures();

			// bind the vertex buffers
			auto passAccess = PassAccess(params->pass);
			for (int i = 0; i < passAccess.getVertexSpecificationCount(); ++i)
			{
				int buffer_index = (int)passAccess.getVertexSpecification()[i].location;
				int buffer_binding = (int)passAccess.getVertexSpecification()[i].binding;
				if (meshBuffer->m_bufferEnabled[buffer_index])
					gfx->setVertexBuffer(buffer_binding, &meshBuffer->m_buffer[buffer_index], 0);
				else
					gfx->setVertexBuffer(buffer_binding, &RrRenderer::Active->GetDefaultVertexBuffer(), 0);
			}

			// bind the index buffer
			gfx->setIndexBuffer(&meshBuffer->m_indexBuffer, gpu::kIndexFormatUnsigned16);
			// bind the cbuffers
			gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
			gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
			gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
			gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
			gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
			gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_USER0, &offset_info);
			// bind the sbuffers
			gfx->setShaderSBuffer(gpu::kShaderStageVs, renderer::SBUFFER_USER0, &instancing_info.buffer_transforms);
			gfx->setShaderSBuffer(gpu::kShaderStageVs, renderer::SBUFFER_USER1, &instancing_info.buffer_variations);

			// draw now
			gfx->drawIndexedInstanced(meshBuffer->m_modeldata->indexNum, subdrawInfo.instance_count, 0, 0);

			// free temp buffer
			offset_info.free(NULL);
		}
	}

	// Success!
	return true;
}