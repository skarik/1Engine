#include "InstancedGrassRenderObject.h"

#include "gpuw/Device.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/Material.h"
#include "renderer/material/RrShaderProgram.h"

InstancedGrassRenderObject::InstancedGrassRenderObject ( void )
	: RrRenderObject()
{
	// Generate a simple grass mesh
	// TODO: Load meshes from disk instead.

	const Real kWidth = 0.1F;
	const Real kHeight = 1.0F;

	arModelData model;

	model.vertexNum = 8;
	model.position = new Vector3f [model.vertexNum];
	model.normal = new Vector3f [model.vertexNum];
	model.tangent = new Vector3f [model.vertexNum];
	model.color = new Vector4f [model.vertexNum];
	model.texcoord0 = new Vector3f [model.vertexNum];

	// set commons
	for ( uint i = 0; i < model.vertexNum; i += 1 )
	{
		model.normal[i] = Vector3f(0, 0, 1.0F);
		model.color[i] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
		model.tangent[i] = Vector3f(1.0F, 0, 0);
	}
	
	// Set UVs:
	for (uint i = 0; i < 2; ++i)
	{
		model.texcoord0[0 + 4*i] = Vector2f(0, 1);
		model.texcoord0[1 + 4*i] = Vector2f(1, 1);
		model.texcoord0[2 + 4*i] = Vector2f(0, 0);
		model.texcoord0[3 + 4*i] = Vector2f(1, 0);
	}

	// Set positions:
	model.position[0] = Vector3f(-kWidth, 0, 0);
	model.position[1] = Vector3f( kWidth, 0, 0);
	model.position[2] = Vector3f(-kWidth, 0, kHeight);
	model.position[3] = Vector3f( kWidth, 0, kHeight);

	model.position[4] = Vector3f(0, -kWidth, 0);
	model.position[5] = Vector3f(0,  kWidth, 0);
	model.position[6] = Vector3f(0, -kWidth, kHeight);
	model.position[7] = Vector3f(0,  kWidth, kHeight);

	// Set triangles
	model.indices = new uint16_t [9];
	model.indexNum = 9;

	model.indices[0] = 0;
	model.indices[1] = 1;
	model.indices[2] = 2;
	model.indices[3] = 3;
	model.indices[4] = 0xFFFF;
	model.indices[5] = 4;
	model.indices[6] = 5;
	model.indices[7] = 6;
	model.indices[8] = 7;

	// Model is created, we upload:
	m_meshBuffer.InitMeshBuffers(&model);
	m_meshBuffer.m_modeldata = NULL;

	// Free the CPU model data:
	delete_safe_array(model.position);
	delete_safe_array(model.normal);
	delete_safe_array(model.tangent);
	delete_safe_array(model.color);
	delete_safe_array(model.texcoord0);

	delete_safe_array(model.indices);

	m_indexCount = model.indexNum;


	// Use a default material
	RrPass pass;
	pass.utilSetupAsDefault();
	pass.m_type = kPassTypeForward;
	pass.m_alphaMode = renderer::kAlphaModeAlphatest;
	pass.m_cullMode = gpu::kCullModeNone;
	pass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/env/foliage_hapgrass0_vv.spv", "shaders/env/foliage_hapgrass0_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	pass.setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));
	pass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	//pass.setTexture(rrTextureSlot::TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite));
	pass.setTexture(rrTextureSlot::TEX_DIFFUSE, RrTexture::Load("textures/foliage/hapgrass0.png"));
	gpu::SamplerCreationDescription pointFilter;
	pointFilter.minFilter = core::gfx::tex::kSamplingPoint;
	pointFilter.magFilter = core::gfx::tex::kSamplingPoint;
	pass.setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);

	PassInitWithInput(0, &pass);
}

InstancedGrassRenderObject::~InstancedGrassRenderObject ( void )
{ 
	m_meshBuffer.FreeMeshBuffers();
}

bool InstancedGrassRenderObject::PreRender ( rrCameraPass* cameraPass )
{
	PushCbufferPerObject(XrTransform(), cameraPass);

	// Loop through the grass and add them to the instancing arrays
	instancing_info.instance_count = 0;
	for (int32 grassIndex = 0; grassIndex < m_grassInfo.size(); ++grassIndex)
	{
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
	}

	ARCORE_ASSERT(instancing_info.instance_count <= instancing_info.variations.size());
	ARCORE_ASSERT(instancing_info.instance_count <= instancing_info.transforms.size());

	instancing_info.buffer_transforms.initAsStructuredBuffer(NULL, sizeof(grInstancedDataGrassTransform) * instancing_info.instance_count);
	instancing_info.buffer_transforms.upload(NULL, instancing_info.transforms.data(), sizeof(grInstancedDataGrassTransform) * instancing_info.instance_count, gpu::kTransferStream);

	instancing_info.buffer_variations.initAsStructuredBuffer(NULL, sizeof(grInstancedDataGrassVariation) * instancing_info.instance_count);
	instancing_info.buffer_variations.upload(NULL, instancing_info.variations.data(), sizeof(grInstancedDataGrassVariation) * instancing_info.instance_count, gpu::kTransferStream);

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
		if ( !m_meshBuffer.m_mesh_uploaded && instancing_info.instance_count && m_indexCount )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = params->context_graphics;

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
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
			if (m_meshBuffer.m_bufferEnabled[buffer_index])
				gfx->setVertexBuffer(buffer_binding, &m_meshBuffer.m_buffer[buffer_index], 0);
		}

		// bind the index buffer
		gfx->setIndexBuffer(&m_meshBuffer.m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// bind the sbuffers
		gfx->setShaderSBuffer(gpu::kShaderStageVs, renderer::SBUFFER_USER0, &instancing_info.buffer_transforms);
		gfx->setShaderSBuffer(gpu::kShaderStageVs, renderer::SBUFFER_USER1, &instancing_info.buffer_variations);

		// draw now
		gfx->drawIndexedInstanced(m_indexCount, instancing_info.instance_count, 0, 0);
	}

	// Success!
	return true;
}