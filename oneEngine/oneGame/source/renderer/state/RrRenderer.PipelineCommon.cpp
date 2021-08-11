//#include "RrPipelinePasses.h"
#include "renderer/state/RrRenderer.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrPass.Presets.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/meshmodel/Loader.h"
#include "core/mem.h"

#include "gpuw/Device.h"
#include "gpuw/Buffers.h"
#include "gpuw/Pipeline.h"

#include "core/math/Math3d.h"

void RrRenderer::InitializeCommonPipelineResources ( gpu::Device* device )
{
	// Create the quad
	Vector4f screenquad [] = {
		// positions
		Vector2f(-1, -1),
		Vector2f(1, -1),
		Vector2f(-1, 1),
		Vector2f(1, 1),
		// uvs
		Vector2f(0, 0),
		Vector2f(1, 0),
		Vector2f(0, 1),
		Vector2f(1, 1),
		// colors
		Vector4f(1, 1, 1, 1),
	};

	// Create a vbuf with flipped output for DX
#if defined(GPU_API_DIRECTX11) || defined(GPU_API_DIRECTX12)
	screenquad[4].y = 1;
	screenquad[5].y = 1;
	screenquad[6].y = 0;
	screenquad[7].y = 0;
#endif

	m_vbufScreenQuad.initAsVertexBuffer(device, gpu::kFormatR32G32B32A32SFloat, sizeof(screenquad)/sizeof(Vector4f));
	m_vbufScreenQuad.upload(NULL, screenquad, sizeof(screenquad), gpu::kTransferStatic);
	
	m_vbufScreenQuad_ForOutputSurface.initAsVertexBuffer(device, gpu::kFormatR32G32B32A32SFloat, sizeof(screenquad)/sizeof(Vector4f));
	m_vbufScreenQuad_ForOutputSurface.upload(NULL, screenquad, sizeof(screenquad), gpu::kTransferStatic);

	CreatePipeline(&renderer::pass::Copy->m_program->GetShaderPipeline(), m_pipelineScreenQuadCopy);

	// Create default geometry for empty values
	Vector4f defaultgeo [] = {
		Vector4f(1, 1, 1, 1),
		Vector4f(0, 0, 0, 1)
	};
	m_vbufDefault.initAsVertexBuffer(device, gpu::kFormatR32G32B32A32SFloat, sizeof(defaultgeo)/sizeof(Vector4f));
	m_vbufDefault.upload(NULL, defaultgeo, sizeof(defaultgeo), gpu::kTransferStatic);

	// Load in lighting geometry
	{
		rrModelLoadParams loadParams {"models/system/light_sphere"};
		RrAnimatedMeshGroup* meshGroup = renderer::LoadMeshGroup(loadParams);
		m_vbufLightSphere = meshGroup->m_meshes[0]->m_buffer[(uint32_t)renderer::shader::Location::kPosition];
	}
	{
		rrModelLoadParams loadParams {"models/system/light_cone"};
		RrAnimatedMeshGroup* meshGroup = renderer::LoadMeshGroup(loadParams);
		m_vbufLightCone = meshGroup->m_meshes[0]->m_buffer[(uint32_t)renderer::shader::Location::kPosition];
	}
}

void RrRenderer::FreeCommonPipelineResources ( gpu::Device* device )
{
	m_vbufScreenQuad.free(device);
	m_pipelineScreenQuadCopy.destroy(device);

	// TODO: Improve this reference removal. LoadMeshGroup adds a reference on load. Perhaps a "retrieval"-only routine instead?
	{
		rrModelLoadParams loadParams {"models/system/light_sphere"};
		RrAnimatedMeshGroup* meshGroup = renderer::LoadMeshGroup(loadParams);
		meshGroup->RemoveReference();
	}

	{
		rrModelLoadParams loadParams {"models/system/light_cone"};
		RrAnimatedMeshGroup* meshGroup = renderer::LoadMeshGroup(loadParams);
		meshGroup->RemoveReference();
	}
}

void RrRenderer::CreatePipeline ( gpu::ShaderPipeline* in_pipeline, gpu::Pipeline& out_pipeline )
{
	// create the pipeline
	gpu::VertexInputBindingDescription binding_desc [2];
	binding_desc[0].binding = 0;
	binding_desc[0].stride = sizeof(Vector4f);
	binding_desc[0].inputRate = gpu::kInputRatePerVertex;
	binding_desc[1].binding = 1;
	binding_desc[1].stride = 0; // no stride, constant color
	binding_desc[1].inputRate = gpu::kInputRatePerVertex;

	gpu::VertexInputAttributeDescription attrib_desc [3];
	attrib_desc[0].binding = 0;
	attrib_desc[0].offset = 0;
	attrib_desc[0].location = (uint32_t)renderer::shader::Location::kPosition;
	attrib_desc[0].format = gpu::kFormatR32G32B32SFloat;
	attrib_desc[1].binding = 0;
	attrib_desc[1].offset = sizeof(Vector4f) * 4;
	attrib_desc[1].location = (uint32_t)renderer::shader::Location::kUV0;
	attrib_desc[1].format = gpu::kFormatR32G32SFloat;
	attrib_desc[2].binding = 1; // uses the binding w/o any stride
	attrib_desc[2].offset = sizeof(Vector4f) * 8;
	attrib_desc[2].location = (uint32_t)renderer::shader::Location::kColor;
	attrib_desc[2].format = gpu::kFormatR32G32B32A32SFloat;

	gpu::PipelineCreationDescription desc;
	desc.shader_pipeline = in_pipeline;
	desc.vv_inputBindings = binding_desc;
	desc.vv_inputBindingsCount = 2;
	desc.vv_inputAttributes = attrib_desc;
	desc.vv_inputAttributesCount = 3;
	desc.ia_topology = gpu::kPrimitiveTopologyTriangleStrip;
	desc.ia_primitiveRestartEnable = false;
	out_pipeline.create(NULL, &desc);
}