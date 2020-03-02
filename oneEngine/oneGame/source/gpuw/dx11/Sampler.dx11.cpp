#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./Sampler.dx11.h"
#include "./Device.dx11.h"
#include "./Internal/Enums.dx11.h"
#include "gpuw/Public/Error.h"

int gpu::Sampler::create ( Device* device, const SamplerCreationDescription* params )
{
	if (m_sampler != NULL)
		static_cast<ID3D11SamplerState*>(m_sampler)->Release();

	D3D11_SAMPLER_DESC samplerInfo = {};
	samplerInfo.Filter = gpu::internal::ArEnumToDx(params->magFilter, params->minFilter, params->mipmapMode, params->anisotropy);
	samplerInfo.AddressU = gpu::internal::ArEnumToDx(params->wrapmodeX);
	samplerInfo.AddressV = gpu::internal::ArEnumToDx(params->wrapmodeY);
	samplerInfo.AddressW = gpu::internal::ArEnumToDx(params->wrapmodeZ);
	samplerInfo.MipLODBias = params->mipmapLodBias;
	samplerInfo.MaxAnisotropy = params->anisotropy ? ((UINT)params->maxAnisotropy) : 1;
	samplerInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerInfo.BorderColor[0] = params->borderColor[0];
	samplerInfo.BorderColor[1] = params->borderColor[1];
	samplerInfo.BorderColor[2] = params->borderColor[2];
	samplerInfo.BorderColor[3] = params->borderColor[3];
	samplerInfo.MinLOD = params->mipmapMinLod;
	samplerInfo.MaxLOD = params->mipmapMaxLod;

	if (device == NULL) device = getDevice();
	device->getNative()->CreateSamplerState(&samplerInfo, (ID3D11SamplerState**)&m_sampler);

	return kError_SUCCESS;
}
int gpu::Sampler::destroy ( Device* device )
{
	if (m_sampler != NULL)
		static_cast<ID3D11SamplerState*>(m_sampler)->Release();
	m_sampler = NULL;
	return kError_SUCCESS;
}

//	valid() : is this sampler valid to be used?
bool gpu::Sampler::valid ( void )
{
	return m_sampler != NULL;
}
//	nativePtr() : returns native index or pointer to the resource.
gpuHandle gpu::Sampler::nativePtr ( void )
{
	return (gpuHandle)(intptr_t)m_sampler;
}


#endif