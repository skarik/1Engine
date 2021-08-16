#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "WriteableResource.dx11.h"

#include "./Device.dx11.h"
#include "./Texture.dx11.h"
#include "./Buffers.dx11.h"
#include "./Internal/Enums.dx11.h"
#include "./BaseContext.dx11.h"
#include "gpuw/Public/Error.h"

bool gpu::WriteableResource::valid ( void )
{
	return m_texture != NULL;
}
gpuHandle gpu::WriteableResource::nativePtr ( void )
{
	return (gpuHandle)m_uav;
}

int gpu::WriteableResource::create ( Texture* texture, const uint32 mipIndex )
{
	ID3D11Device*	device = gpu::getDevice()->getNative();
	HRESULT			result;

	m_texture = texture;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavInfo;

	switch (texture->m_type)
	{
	case core::gfx::tex::kTextureType2D:
	case core::gfx::tex::kTextureType2DArray:
	case core::gfx::tex::kTextureTypeCube:
	case core::gfx::tex::kTextureTypeCubeArray:
		uavInfo.Format = gpu::internal::ArEnumToDx(texture->m_format, false, false);
		if (texture->m_type == core::gfx::tex::kTextureType2D)
		{
			uavInfo.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavInfo.Texture2D.MipSlice = mipIndex;
		}
		else if (texture->m_type == core::gfx::tex::kTextureType2DArray)
		{
			uavInfo.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uavInfo.Texture2DArray.MipSlice = mipIndex;
			uavInfo.Texture2DArray.FirstArraySlice = 0;
			uavInfo.Texture2DArray.ArraySize = texture->m_depth;
		}
		else if (texture->m_type == core::gfx::tex::kTextureTypeCube)
		{
			uavInfo.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uavInfo.Texture2DArray.MipSlice = mipIndex;
			uavInfo.Texture2DArray.FirstArraySlice = 0;
			uavInfo.Texture2DArray.ArraySize = texture->m_depth;
		}
		else if (texture->m_type == core::gfx::tex::kTextureTypeCubeArray)
		{
			uavInfo.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uavInfo.Texture2DArray.MipSlice = mipIndex;
			uavInfo.Texture2DArray.FirstArraySlice = 0;
			uavInfo.Texture2DArray.ArraySize = texture->m_depth;
		}

		result = device->CreateUnorderedAccessView((ID3D11Texture2D*)texture->m_texture, &uavInfo, (ID3D11UnorderedAccessView**)&m_uav);
		if (FAILED(result))
			throw core::OutOfMemoryException(); // TODO: Handle this better.

		break;

	default:
		throw core::NotYetImplementedException();
	}
	return gpu::kError_SUCCESS;
}


int gpu::WriteableResource::destroy ( void )
{
	if (m_uav)
		static_cast<ID3D11UnorderedAccessView*>(m_uav)->Release();

	m_uav = NULL;

	return gpu::kError_SUCCESS;
}

#endif