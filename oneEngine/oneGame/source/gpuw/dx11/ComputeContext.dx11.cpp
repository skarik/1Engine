#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "ComputeContext.dx11.h"
#include "gpuw/Public/Error.h"

#include "core/debug.h"

/*#include "./RenderTarget.dx11.h"
#include "./Pipeline.dx11.h"
#include "./ShaderPipeline.dx11.h"
#include "./Sampler.dx11.h"
#include "./Buffers.dx11.h"
#include "./Fence.dx11.h"

#include "./Internal/Enums.h"*/

//#include "./ogl/GLCommon.h"
#include "./Device.dx11.h"

#include <stdio.h>


gpu::ComputeContext::ComputeContext ( void* wrapperDevice )
{
	HRESULT			result;
	gpu::Device*	gpuDevice = (gpu::Device*)wrapperDevice;

	// Save the device for submit
	m_wrapperDevice = wrapperDevice;

	// Grab a new context
	result = gpuDevice->getNative()->CreateDeferredContext(0, (ID3D11DeviceContext**)&m_deferredContext);
	if (FAILED(result))
	{
		// TODO: Error handling.
	}
}

gpu::ComputeContext::~ComputeContext ( void )
{
	static_cast<ID3D11DeviceContext*>(m_deferredContext)->Release();
}

int gpu::ComputeContext::reset ( void )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	ctx->ClearState(); // TODO: Remove this line, and investigate if it causes issues, if any.

	return gpu::kError_SUCCESS;
}

int gpu::ComputeContext::submit ( void )
{
	gpu::Device*			gpuDevice = (gpu::Device*)m_wrapperDevice;
	ID3D11CommandList*		commandList = NULL;
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	ctx->FinishCommandList(FALSE, &commandList); // Finalize all commands thrown in.
	gpuDevice->getNativeContext()->ExecuteCommandList(commandList, FALSE);

	return gpu::kError_SUCCESS;
}

int gpu::ComputeContext::validate ( void )
{
	return gpu::kError_SUCCESS;
}

#endif