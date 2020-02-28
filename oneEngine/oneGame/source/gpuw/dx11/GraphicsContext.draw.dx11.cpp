#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "GraphicsContext.dx11.h"
#include "gpuw/Public/Error.h"

#include "core/debug.h"

//#include "./RenderTarget.dx11.h"
#include "./Pipeline.dx11.h"
#include "./ShaderPipeline.dx11.h"
//#include "./Sampler.dx11.h"
#include "./Buffers.dx11.h"
//#include "./Fence.dx11.h"

#include "./Internal/Enums.dx11.h"

//#include "./ogl/GLCommon.h"
#include "./Device.dx11.h"

#include <stdio.h>


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

		ctx->IASetIndexBuffer((ID3D11Buffer*)m_indexBuffer->nativePtr(), (m_indexFormat == kIndexFormatUnsigned16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
		ctx->IASetPrimitiveTopology(gpu::internal::ArEnumToDx(m_pipeline->ia_topology));
		// Primitive restart is always enabled on DX11.

		// All other buffers are set before this point. Let DX11 handle the context changes.

		// Set the layout we're going to use last
		ctx->IASetInputLayout((ID3D11InputLayout*)m_pipeline->m_layout);

		// TODO: loop through the vertex buffers & make sure the pipeline is setup properly.
		//		 dx11 has proper error handling for this case, but opengl does not as it forces a driver restart
		m_pipelineBound = true;
		return kError_SUCCESS;
	}
	return kError_SUCCESS;
}


#endif