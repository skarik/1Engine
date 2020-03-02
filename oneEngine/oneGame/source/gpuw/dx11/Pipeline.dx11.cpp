#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./Pipeline.dx11.h"
#include "./ShaderPipeline.dx11.h"
#include "./Shader.dx11.h"
#include "./Device.dx11.h"
#include "./Internal/Enums.dx11.h"
#include "gpuw/Public/Error.h"

#include "core/debug.h"
#include "core/debug/console.h"

int gpu::Pipeline::create ( Device* device, const PipelineCreationDescription* params )
{
	D3D11_INPUT_ELEMENT_DESC layoutTable [D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	if (device == NULL) device = getDevice();

	// Generate the layout table:
	for (uint32_t i = 0; i < params->vv_inputAttributesCount; ++i)
	{
		// Find the matching binding info
		uint32_t ibinding_target = 0;
		for (uint32_t ibinding = 0; ibinding < params->vv_inputBindingsCount; ++ibinding)
		{
			if (params->vv_inputBindings[ibinding].binding == params->vv_inputAttributes[i].binding)
			{
				ibinding_target = ibinding;
				break;
			}
		}

		// Create the combined element info
		D3D11_INPUT_ELEMENT_DESC* va = &layoutTable[i];
		va->SemanticName	= "TEXCOORD";
		va->SemanticIndex	= params->vv_inputAttributes[i].location;
		va->Format			= gpu::internal::ArEnumToDx(params->vv_inputAttributes[i].format);
		va->InputSlot		= params->vv_inputAttributes[i].binding;
		va->AlignedByteOffset		= params->vv_inputAttributes[i].offset;
		va->InputSlotClass	= gpu::internal::ArEnumToDx(params->vv_inputBindings[ibinding_target].inputRate);
		va->InstanceDataStepRate	= (params->vv_inputBindings[ibinding_target].inputRate == kInputRatePerVertex) ? 0 : 1;
	}

	// Create the layout:
	device->getNative()->CreateInputLayout(layoutTable, params->vv_inputAttributesCount,
										   params->shader_pipeline->m_shaderVs->m_shaderBytes, 
										   params->shader_pipeline->m_shaderVs->m_shaderLength,
										   (ID3D11InputLayout**)&m_layout);

	// save pipeline & primitive settings
	m_pipeline = params->shader_pipeline;
	ia_topology = params->ia_topology;
	ia_primitiveRestartEnable = params->ia_primitiveRestartEnable;

	// copy binding info over, since it is needed for ctx::IASetVertexBuffers
	ia_bindingInfo = new VertexInputBindingDescription[params->vv_inputBindingsCount];
	memcpy(ia_bindingInfo, params->vv_inputBindings, sizeof(VertexInputBindingDescription) * params->vv_inputBindingsCount);
	ia_bindingInfoCount = params->vv_inputBindingsCount;

	return kError_SUCCESS;
}

int gpu::Pipeline::destroy ( Device* device )
{
	if (m_layout)
		static_cast<ID3D11InputLayout*>(m_layout)->Release();
	if (ia_bindingInfo)
		delete[] ia_bindingInfo;

	m_layout = NULL;
	ia_bindingInfo = NULL;

	return kError_SUCCESS;
}

//	nativePtr() : returns native index or pointer to the resource.
gpuHandle gpu::Pipeline::nativePtr ( void )
{
	return (intptr_t)m_layout;
}

//	valid() : is this pipeline valid to be used?
// If the pipeline failed to be created or doesnt exist, this will be false
bool gpu::Pipeline::valid ( void )
{
	return m_layout != NULL && m_pipeline != NULL;
}

#endif