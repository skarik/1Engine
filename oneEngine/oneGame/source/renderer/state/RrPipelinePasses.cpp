#include "RrPipelinePasses.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrPass.Presets.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/texture/RrTexture.h"
#include "core/mem.h"

#include "gpuw/Device.h"
#include "gpuw/Buffers.h"
#include "gpuw/Pipeline.h"

#include "core/math/Math3d.h"

renderer::pipeline::RrPipelinePasses::RrPipelinePasses ( void )
{
	// Create the render copy upscaling shader:
	CopyScaled = new RrPass();
	// Setup forward pass
	CopyScaled->m_type = kPassTypeForward;
	CopyScaled->m_cullMode = gpu::kCullModeNone;
	CopyScaled->m_depthTest = gpu::kCompareOpAlways;
	CopyScaled->setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{
		"shaders/sys/copy_buffer_scaled_vv.spv",
		"shaders/sys/copy_buffer_scaled_p.spv"}) );

	// Create the passes for rendering the screen:
	LightingPass = new RrPass();
	// Setup forward pass
	LightingPass->m_type = kPassTypeForward;
	LightingPass->m_cullMode = gpu::kCullModeNone;
	LightingPass->setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{
		"shaders/def_screen/pass_lighting_world_vv.spv",
		"shaders/def_screen/pass_lighting_world_p.spv"}) );

	EchoPass = new RrPass();
	// Setup forward pass
	EchoPass->m_type = kPassTypeForward;
	EchoPass->m_cullMode = gpu::kCullModeNone;
	EchoPass->setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{
		"shaders/def_screen/pass_lighting_echo_vv.spv",
		"shaders/def_screen/pass_lighting_echo_p.spv"}) );

	ShaftPass = new RrPass();
	// Setup forward pass
	ShaftPass->m_type = kPassTypeForward;
	ShaftPass->m_cullMode = gpu::kCullModeNone;
	ShaftPass->setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{
		"shaders/def_screen/pass_lighting_shaft_vv.spv",
		"shaders/def_screen/pass_lighting_shaft_p.spv"}) );
	// Set effect textures
	ShaftPass->setTexture( TEX_SLOT5, RrTexture::Load( "textures/ditherdots.jpg" ) );
	
	Lighting2DPass = new RrPass();
	// Setup forward pass
	Lighting2DPass->m_type = kPassTypeForward;
	Lighting2DPass->m_cullMode = gpu::kCullModeNone;
	Lighting2DPass->setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{
		"shaders/def_screen/pass_lighting2d_vv.spv",
		"shaders/def_screen/pass_lighting2d_p.spv"}) );

	// Create the quad
	Vector4f screenquad [] = {
		// positions
		Vector2f(1, 1),
		Vector2f(-1, 1),
		Vector2f(1, -1),
		Vector2f(-1, -1),
		// uvs
		Vector2f(1, 1),
		Vector2f(0, 1),
		Vector2f(1, 0),
		Vector2f(0, 0),
		// colors
		Vector4f(1, 1, 1, 1),
	};

	m_vbufScreenQuad.initAsVertexBuffer(NULL, gpu::kFormatR32G32B32A32SFloat, sizeof(screenquad)/sizeof(Vector4f));
	m_vbufScreenQuad.upload(NULL, screenquad, sizeof(screenquad), gpu::kTransferStatic);
	
	// Create a vbuf with flipped output for DX
#if defined(GPU_API_DIRECTX11) || defined(GPU_API_DIRECTX12)
	screenquad[4].y = 0;
	screenquad[5].y = 0;
	screenquad[6].y = 1;
	screenquad[7].y = 1;
#endif
	m_vbufScreenQuad_ForOutputSurface.initAsVertexBuffer(NULL, gpu::kFormatR32G32B32A32SFloat, sizeof(screenquad)/sizeof(Vector4f));
	m_vbufScreenQuad_ForOutputSurface.upload(NULL, screenquad, sizeof(screenquad), gpu::kTransferStatic);

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
	desc.shader_pipeline = &renderer::pass::Copy->m_program->GetShaderPipeline();
	desc.vv_inputBindings = binding_desc;
	desc.vv_inputBindingsCount = 2;
	desc.vv_inputAttributes = attrib_desc;
	desc.vv_inputAttributesCount = 3;
	desc.ia_topology = gpu::kPrimitiveTopologyTriangleStrip;
	desc.ia_primitiveRestartEnable = false;
	m_pipelineScreenQuadCopy.create(NULL, &desc);
}

renderer::pipeline::RrPipelinePasses::~RrPipelinePasses ( void )
{
	delete_safe(CopyScaled);

	delete_safe(LightingPass);
	delete_safe(EchoPass);
	delete_safe(ShaftPass);
	delete_safe(Lighting2DPass);

	m_vbufScreenQuad.free(NULL);
	m_pipelineScreenQuadCopy.destroy(NULL);
}