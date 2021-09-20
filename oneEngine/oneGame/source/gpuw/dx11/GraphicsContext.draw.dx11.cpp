#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "GraphicsContext.dx11.h"
#include "gpuw/Public/Error.h"

#include "core/debug.h"

#include "./RenderTarget.dx11.h"
#include "./Pipeline.dx11.h"
#include "./ShaderPipeline.dx11.h"
#include "./Sampler.dx11.h"
#include "./Texture.dx11.h"
#include "./Buffers.dx11.h"
#include "./WriteableResource.dx11.h"
//#include "./Fence.dx11.h"

#include "./Internal/Enums.dx11.h"

//#include "./ogl/GLCommon.h"
#include "./Device.dx11.h"

#include <stdio.h>

int gpu::GraphicsContext::clearDepthStencil ( bool clearDepth, float depth, bool clearStencil, uint8_t stencil )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	if (m_depthStencilTarget != NULL)
	{
		ctx->ClearDepthStencilView((ID3D11DepthStencilView*)m_depthStencilTarget,
									(clearDepth ? D3D11_CLEAR_DEPTH : 0) | (clearStencil ? D3D11_CLEAR_STENCIL : 0),
									depth,
									stencil);
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::clearColor ( float* rgbaColor )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	// TODO: Reimplement with screen triangle

	if (m_renderTarget[0] != NULL)
	{
		ctx->ClearRenderTargetView((ID3D11RenderTargetView*)m_renderTarget[0], rgbaColor);
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::clearColorAll ( float* rgbaColor )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	// TODO: Reimplement with screen triangle

	for (uint i = 0; i < 16; ++i)
	{
		if (m_renderTarget[i] != NULL)
		{
			ctx->ClearRenderTargetView((ID3D11RenderTargetView*)m_renderTarget[i], rgbaColor);
		}
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setRenderTarget ( RenderTarget* renderTarget )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	if (renderTarget != NULL)
	{
		// We through the attachments on the render target to generate the list of actual targets.
		// TODO: This could be pregenerated on the RenderTarget themselves on their assembly. This would also allow removing the awful 0xFFFFFFFF hack.
		UINT attachmentCount = 0;
		ID3D11RenderTargetView* attachments [16] = {};

		for (UINT i = 0; i < 16; ++i)
		{
			if (renderTarget->m_attachments[i] != NULL)
			{
				attachments[attachmentCount] = (ID3D11RenderTargetView*)renderTarget->m_attachments[i];
				++attachmentCount;
			}
			else
			{
				// Since RTs cannot be written to in certain bind cases, allow for explicit unbinding.
				attachments[attachmentCount] = NULL;
			}
		}

		ctx->OMSetRenderTargets(attachmentCount, attachments, (ID3D11DepthStencilView*)renderTarget->m_attachmentDepthStencil);
		m_depthStencilTarget = renderTarget->m_attachmentDepthStencil;

		memcpy(m_renderTarget, attachments, sizeof(void*) * 16);
	}
	else
	{
		ctx->OMSetRenderTargets(0, NULL, NULL);
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::clearPipelineAndWait ( void )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	// TODO: move this elsewhere if possible
	// Unbind all the resources now
	void* nullRez [128] = {NULL};
	ctx->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)nullRez);
	ctx->HSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)nullRez);
	ctx->DSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)nullRez);
	ctx->GSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)nullRez);
	ctx->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)nullRez);
	ctx->CSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)nullRez);

	ctx->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, (ID3D11ShaderResourceView**)nullRez);
	ctx->HSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, (ID3D11ShaderResourceView**)nullRez);
	ctx->DSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, (ID3D11ShaderResourceView**)nullRez);
	ctx->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, (ID3D11ShaderResourceView**)nullRez);
	ctx->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, (ID3D11ShaderResourceView**)nullRez);
	ctx->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, (ID3D11ShaderResourceView**)nullRez);

	ctx->CSSetUnorderedAccessViews(0, D3D11_1_UAV_SLOT_COUNT, (ID3D11UnorderedAccessView**)nullRez, (UINT*)nullRez);

	ctx->IASetVertexBuffers(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, (ID3D11Buffer**)nullRez, (UINT*)nullRez, (UINT*)nullRez);

	m_renderTarget[0] = NULL;
	m_depthStencilTarget = NULL;

	memset(m_constantBuffers, 0, sizeof(m_constantBuffers));

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setPipeline ( const Pipeline* pipeline )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	if (pipeline != m_pipeline)
	{
		m_pipeline = (Pipeline*)pipeline;
		m_pipelineBound = false;
		m_pipelineDataBound = false;

		m_indexBuffer = NULL;
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setIndexBuffer ( const Buffer* buffer, IndexFormat format )
{
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeIndex);
	ARCORE_ASSERT(m_pipeline != NULL);

	// TODO:

	m_indexBuffer = (Buffer*)buffer;
	m_indexFormat = format;

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setVertexBuffer ( int slot, const Buffer* buffer, uint32_t offset )
{
	// todo: optimize later
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeVertex);
	ARCORE_ASSERT(m_pipeline != NULL);
	//ARCORE_ASSERT(slot < (int)m_pipeline->ia_bindingInfoCount); // Incorrect with wierd vertex layouts.

	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	ID3D11Buffer* bufferList [1] = {(ID3D11Buffer*)((Buffer*)buffer)->nativePtr()};
	UINT strideList [1] = {m_pipeline->ia_bindingInfo[slot].stride};
	UINT offsetList [1] = {0};

	ctx->IASetVertexBuffers(slot, 1, bufferList, strideList, offsetList);

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderCBuffer ( ShaderStage stage, int slot, const Buffer* buffer )
{
	if (buffer == nullptr)
		return kError_SUCCESS; // TODO: handle this better

	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeConstant);
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	/*ID3D11Buffer* bufferList [1] = {(ID3D11Buffer*)((Buffer*)buffer)->nativePtr()};
	if (stage == kShaderStageVs)
		ctx->VSSetConstantBuffers(slot, 1, bufferList);
	else if (stage == kShaderStageHs)
		ctx->HSSetConstantBuffers(slot, 1, bufferList);
	else if (stage == kShaderStageDs)
		ctx->DSSetConstantBuffers(slot, 1, bufferList);
	else if (stage == kShaderStageGs)
		ctx->GSSetConstantBuffers(slot, 1, bufferList);
	else if (stage == kShaderStagePs)
		ctx->PSSetConstantBuffers(slot, 1, bufferList);
	else if (stage == kShaderStageCs)
		ctx->CSSetConstantBuffers(slot, 1, bufferList);
	else
	{
		ARCORE_ERROR("Invalid shader stage");
	}*/
	m_constantBuffers[stage].m_buffers[slot] = (ID3D11Buffer*)((Buffer*)buffer)->nativePtr();

	// todo: bind
	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderCBuffers ( ShaderStage stage, int startSlot, int slotCount, Buffer* const* buffers )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	ARCORE_ASSERT(startSlot >= 0);
	ARCORE_ASSERT(slotCount + startSlot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);

	// Build list of shaders we're passing in
	/*ID3D11Buffer* bufferList [D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {NULL};
	for ( int i = 0; i < slotCount; ++i )
	{
		if (buffers[i])
		{
			ARCORE_ASSERT(buffers[i]->getBufferType() == kBufferTypeConstant);
			bufferList[i] = (ID3D11Buffer*)((Buffer*)buffers[i])->nativePtr();
		}
	}

	if (stage == kShaderStageVs)
		ctx->VSSetConstantBuffers(startSlot, slotCount, bufferList);
	else if (stage == kShaderStageHs)
		ctx->HSSetConstantBuffers(startSlot, slotCount, bufferList);
	else if (stage == kShaderStageDs)
		ctx->DSSetConstantBuffers(startSlot, slotCount, bufferList);
	else if (stage == kShaderStageGs)
		ctx->GSSetConstantBuffers(startSlot, slotCount, bufferList);
	else if (stage == kShaderStagePs)
		ctx->PSSetConstantBuffers(startSlot, slotCount, bufferList);
	else if (stage == kShaderStageCs)
		ctx->CSSetConstantBuffers(startSlot, slotCount, bufferList);
	else
	{
		ARCORE_ERROR("Invalid shader stage");
	}*/
	for ( int i = 0; i < slotCount; ++i )
	{
		if (buffers[i])
		{
			ARCORE_ASSERT(buffers[i]->getBufferType() == kBufferTypeConstant);
			m_constantBuffers[stage].m_buffers[startSlot + i] = (ID3D11Buffer*)((Buffer*)buffers[i])->nativePtr();
		}
	}

	// todo: bind
	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderSBuffer ( ShaderStage stage, int slot, Buffer* buffer )
{
	if (buffer == nullptr)
		return kError_SUCCESS; // TODO: handle this better

	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeStructured);
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	// https://docs.microsoft.com/en-us/windows/win32/direct3d12/resource-binding-in-hlsl
	//ctx->OMGetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, NULL, NULL,
	//											   slot, 1, uavViewList);
	
	ID3D11ShaderResourceView* srvList [1] = {(ID3D11ShaderResourceView*)buffer->m_srv};
	if (stage == kShaderStageVs)
		ctx->VSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStageHs)
		ctx->HSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStageDs)
		ctx->DSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStageGs)
		ctx->GSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStagePs)
		ctx->PSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStageCs)
		ctx->CSSetShaderResources(slot, 1, srvList);
	else
	{
		ARCORE_ERROR("Invalid shader stage");
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderSampler ( ShaderStage stage, int slot, Sampler* sampler )
{
	ARCORE_ASSERT(sampler != NULL);
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	ID3D11SamplerState* samplerList [1] = {(ID3D11SamplerState*)sampler->nativePtr()};
	if (stage == kShaderStageVs)
		ctx->VSSetSamplers(slot, 1, samplerList);
	else if (stage == kShaderStageHs)
		ctx->HSSetSamplers(slot, 1, samplerList);
	else if (stage == kShaderStageDs)
		ctx->DSSetSamplers(slot, 1, samplerList);
	else if (stage == kShaderStageGs)
		ctx->GSSetSamplers(slot, 1, samplerList);
	else if (stage == kShaderStagePs)
		ctx->PSSetSamplers(slot, 1, samplerList);
	else if (stage == kShaderStageCs)
		ctx->CSSetSamplers(slot, 1, samplerList);
	else
	{
		ARCORE_ERROR("Invalid shader stage");
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderTexture ( ShaderStage stage, int slot, Texture* texture )
{
	ARCORE_ASSERT(texture != NULL);
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	//glBindTextureUnit(slot, (GLuint)texture->nativePtr());

	ID3D11ShaderResourceView* srvList [1] = {(ID3D11ShaderResourceView*)texture->m_srv};
	if (stage == kShaderStageVs)
		ctx->VSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStageHs)
		ctx->HSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStageDs)
		ctx->DSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStageGs)
		ctx->GSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStagePs)
		ctx->PSSetShaderResources(slot, 1, srvList);
	else if (stage == kShaderStageCs)
		ctx->CSSetShaderResources(slot, 1, srvList);
	else
	{
		ARCORE_ERROR("Invalid shader stage");
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderTexture ( ShaderStage stage, int slot, Texture* texture, Sampler* sampler )
{
	ARCORE_ASSERT(texture != NULL);
	ARCORE_ASSERT(sampler != NULL);

	setShaderSampler( stage, slot, sampler );
	setShaderTexture( stage, slot, texture );

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setShaderTextureAuto ( ShaderStage stage, int slot, Texture* texture )
{
	ARCORE_ASSERT(texture != NULL);

	setShaderSampler(stage, slot, m_defaultSampler);
	setShaderTexture(stage, slot, texture);

	return kError_SUCCESS;
}

// this is based on a playstation-ism - it is never actually used so far
//int gpu::GraphicsContext::setShaderResource ( ShaderStage stage, int slot, Buffer* buffer )
//{
//	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeStructured);
//	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
//
//	ID3D11ShaderResourceView* srvList [1] = {(ID3D11ShaderResourceView*)buffer->nativePtr()};
//	if (stage == kShaderStageVs)
//		ctx->VSSetShaderResources(slot, 1, srvList);
//	else if (stage == kShaderStageHs)
//		ctx->HSSetShaderResources(slot, 1, srvList);
//	else if (stage == kShaderStageDs)
//		ctx->DSSetShaderResources(slot, 1, srvList);
//	else if (stage == kShaderStageGs)
//		ctx->GSSetShaderResources(slot, 1, srvList);
//	else if (stage == kShaderStagePs)
//		ctx->PSSetShaderResources(slot, 1, srvList);
//	else if (stage == kShaderStageCs)
//		ctx->CSSetShaderResources(slot, 1, srvList);
//
//	return kError_SUCCESS;
//}

int gpu::GraphicsContext::setShaderWriteable ( ShaderStage stage, int slot, WriteableResource* resource )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	if (resource == NULL)
	{
		ID3D11UnorderedAccessView* uavList [1] = {NULL};
		ctx->CSSetUnorderedAccessViews(slot, 1, uavList, NULL);
	}
	else
	{
		ARCORE_ASSERT(resource->valid());

		ID3D11UnorderedAccessView* uavList [1] = {(ID3D11UnorderedAccessView*)resource->nativePtr()};
		if (stage == kShaderStagePs)
		{
			throw core::NotYetImplementedException(); // This has to be set during the OM stage for PS.
		}
		else if (stage == kShaderStageCs)
		{
			ctx->CSSetUnorderedAccessViews(slot, 1, uavList, NULL); // TODO: The last argument is for flag buffers & counting buffers.
		}
		else
		{
			ARCORE_ERROR("Invalid shader stage");
		}
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::drawPreparePipeline ( void )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	if (m_pipelineBound == false)
	{
		// Bind shader program to use
		if (m_pipeline->m_pipeline->m_vs)
			ctx->VSSetShader((ID3D11VertexShader*)m_pipeline->m_pipeline->m_vs, NULL, 0);
		if (m_pipeline->m_pipeline->m_hs && m_pipeline->m_pipeline->m_ds)
		{
			ctx->HSSetShader((ID3D11HullShader*)m_pipeline->m_pipeline->m_hs, NULL, 0);
			ctx->DSSetShader((ID3D11DomainShader*)m_pipeline->m_pipeline->m_ds, NULL, 0);
		}
		if (m_pipeline->m_pipeline->m_gs)
			ctx->GSSetShader((ID3D11GeometryShader*)m_pipeline->m_pipeline->m_gs, NULL, 0);
		if (m_pipeline->m_pipeline->m_ps)
			ctx->PSSetShader((ID3D11PixelShader*)m_pipeline->m_pipeline->m_ps, NULL, 0);

		if (m_indexBuffer != NULL)
			ctx->IASetIndexBuffer((ID3D11Buffer*)m_indexBuffer->nativePtr(), (m_indexFormat == kIndexFormatUnsigned16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
		ctx->IASetPrimitiveTopology(gpu::internal::ArEnumToDx(m_pipeline->ia_topology));
		// Primitive restart is always enabled on DX11.

		// All other buffers are set before this point. Let DX11 handle the context changes.
		
		{
			// PSYCH! CBuffer set is a mite slower when letting DX11 handle the context changes.
			// Thus, we go through each bound shader and set their stages' constant buffers
			if (m_pipeline->m_pipeline->m_vs)
				ctx->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageVs].m_buffers);
			if (m_pipeline->m_pipeline->m_hs)
				ctx->HSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageHs].m_buffers);
			if (m_pipeline->m_pipeline->m_ds)
				ctx->DSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageDs].m_buffers);
			if (m_pipeline->m_pipeline->m_gs)
				ctx->GSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageGs].m_buffers);
			if (m_pipeline->m_pipeline->m_ps)
				ctx->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStagePs].m_buffers);
		}

		// Set the layout we're going to use last
		ctx->IASetInputLayout((ID3D11InputLayout*)m_pipeline->m_layout);

		// TODO: loop through the vertex buffers & make sure the pipeline is setup properly.
		//		 dx11 has proper error handling for this case, but opengl does not as it forces a driver restart
		m_pipelineBound = true;
		return kError_SUCCESS;
	}
	else
	{
		// Update the IA state that could have changed if we're reusing the pipeline
		if (m_indexBuffer != NULL)
			ctx->IASetIndexBuffer((ID3D11Buffer*)m_indexBuffer->nativePtr(), (m_indexFormat == kIndexFormatUnsigned16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);

		// Update the CBuffer's for the same reason.
		{
			// PSYCH! CBuffer set is a mite slower when letting DX11 handle the context changes.
			// Thus, we go through each bound shader and set their stages' constant buffers
			if (m_pipeline->m_pipeline->m_vs)
				ctx->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageVs].m_buffers);
			if (m_pipeline->m_pipeline->m_hs)
				ctx->HSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageHs].m_buffers);
			if (m_pipeline->m_pipeline->m_ds)
				ctx->DSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageDs].m_buffers);
			if (m_pipeline->m_pipeline->m_gs)
				ctx->GSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageGs].m_buffers);
			if (m_pipeline->m_pipeline->m_ps)
				ctx->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStagePs].m_buffers);
		}
	}
	return kError_SUCCESS;
}

int gpu::GraphicsContext::draw ( const uint32_t vertexCount, const uint32_t startVertex )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		ctx->Draw(vertexCount, startVertex);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}

int gpu::GraphicsContext::drawInstanced ( const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t startVertex )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		ctx->DrawInstanced(vertexCount, instanceCount, startVertex, 0);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}

int gpu::GraphicsContext::drawIndexed ( const uint32_t indexCount, const uint32_t startIndex, const uint32_t baseVertex )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		ctx->DrawIndexed(indexCount, startIndex, baseVertex);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}

int gpu::GraphicsContext::drawIndexedInstanced ( const uint32_t indexCount, const uint32_t instanceCount, const uint32_t startIndex, const uint32_t baseVertex )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		ctx->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, 0);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}

int	 gpu::GraphicsContext::setIndirectArgs ( Buffer* buffer )
{
	ARCORE_ASSERT(buffer->getBufferType() == kBufferTypeIndirectArgs);
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	m_indirectArgsBuffer = buffer;

	return kError_SUCCESS;
}

int gpu::GraphicsContext::drawInstancedIndirect ( const uint32_t offset )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	if (drawPreparePipeline() == kError_SUCCESS)
	{
		ctx->DrawIndexedInstancedIndirect((ID3D11Buffer*)m_indirectArgsBuffer->nativePtr(), offset);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
	return kErrorBadArgument;
}

int gpu::GraphicsContext::setComputeShader ( ShaderPipeline* computePipeline )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	ctx->CSSetShader((ID3D11ComputeShader*)computePipeline->m_cs, NULL, 0);
	ARCORE_ASSERT(validate() == 0);

	return kError_SUCCESS;
}

int gpu::GraphicsContext::dispatch ( const uint32 threadCountX, const uint32 threadCountY, const uint32 threadCountZ )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	{
		ctx->CSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, (ID3D11Buffer**)m_constantBuffers[kShaderStageCs].m_buffers);
	}
	{
		ctx->Dispatch(threadCountX, threadCountY, threadCountZ);
		ARCORE_ASSERT(validate() == 0);
		return kError_SUCCESS;
	}
}



#endif