#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./RenderTarget.dx11.h"
#include "./Device.dx11.h"
#include "./Internal/Enums.dx11.h"
#include "gpuw/Public/Error.h"
#include "core/debug.h"

gpu::RenderTarget::RenderTarget ( void )
	:
	m_attachments(), m_attachmentDepthStencil(NULL),
	m_assembled(false), m_hasFailure(false)
{}

gpu::RenderTarget::~RenderTarget ( void )
{}

int gpu::RenderTarget::create ( Device* device )
{
	destroy(device);
	return 0;
}

int gpu::RenderTarget::destroy ( Device* device )
{
	for (uint32_t i = 0; i < 16; ++i)
	{
		m_highlevelAttachments[i] = NULL;
		if (m_attachments[i])
			static_cast<ID3D11RenderTargetView*>(m_attachments[i])->Release();
		m_attachments[i] = NULL;
	}

	m_highlevelAttachments[16] = NULL;
	m_highlevelAttachments[17] = NULL;
	if (m_attachmentDepthStencil)
		static_cast<ID3D11DepthStencilView*>(m_attachmentDepthStencil)->Release();
	m_attachmentDepthStencil = NULL;

	m_assembled = false;
	m_hasFailure = false;
	return 0;
}

int gpu::RenderTarget::attach ( int slot, Texture* texture )
{
	ID3D11Device*	device = gpu::getDevice()->getNative();
	HRESULT			result;

	if (texture->m_texture == NULL)
	{
		m_hasFailure = true;
		return kErrorBadArgument;
	}

	if (slot == kRenderTargetSlotDepthStencil)
	{
		ARCORE_ASSERT(m_attachmentDepthStencil == NULL);
		m_highlevelAttachments[15 - slot] = texture;

		D3D11_DEPTH_STENCIL_VIEW_DESC rtInfo = {};
		rtInfo.Format = gpu::internal::ArEnumToDx(texture->m_format, true, false);
		switch (texture->m_type)
		{
		case core::gfx::tex::kTextureType1D:
			rtInfo.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
			rtInfo.Texture1D.MipSlice = 0;
			break;
		case core::gfx::tex::kTextureType1DArray:
			rtInfo.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
			rtInfo.Texture1DArray.MipSlice = 0;
			rtInfo.Texture1DArray.FirstArraySlice = 0;
			rtInfo.Texture1DArray.ArraySize = 0;
			break;
		case core::gfx::tex::kTextureType2D:
			rtInfo.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			rtInfo.Texture2D.MipSlice = 0;
			break;
		case core::gfx::tex::kTextureType2DArray:
			rtInfo.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			rtInfo.Texture2DArray.MipSlice = 0;
			rtInfo.Texture2DArray.FirstArraySlice = 0;
			rtInfo.Texture2DArray.ArraySize = 0;
			break;
		default:
			rtInfo.ViewDimension = D3D11_DSV_DIMENSION_UNKNOWN;
			break;
		}

		result = device->CreateDepthStencilView((ID3D11Resource*)texture->nativePtr(), &rtInfo, (ID3D11DepthStencilView**)&m_attachmentDepthStencil);
		if (FAILED(result)) {
			m_hasFailure = true;
			return kErrorFormatUnsupported;
		}
	}
	else if (slot >= kRenderTargetSlotColor0)
	{
		ARCORE_ASSERT(m_attachments[slot] == NULL);
		m_highlevelAttachments[slot] = texture;

		D3D11_RENDER_TARGET_VIEW_DESC rtInfo = {};
		rtInfo.Format = gpu::internal::ArEnumToDx(texture->m_format, true, false);
		switch (texture->m_type)
		{
		case core::gfx::tex::kTextureTypeNone:
			rtInfo.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
			rtInfo.Buffer.ElementOffset = 0;
			rtInfo.Buffer.ElementWidth = 4;
			break;
		case core::gfx::tex::kTextureType1D:
			rtInfo.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			rtInfo.Texture1D.MipSlice = 0;
			break;
		case core::gfx::tex::kTextureType1DArray:
			rtInfo.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			rtInfo.Texture1DArray.MipSlice = 0;
			rtInfo.Texture1DArray.FirstArraySlice = 0;
			rtInfo.Texture1DArray.ArraySize = 0;
			break;
		case core::gfx::tex::kTextureType2D:
			rtInfo.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtInfo.Texture2D.MipSlice = 0;
			break;
		case core::gfx::tex::kTextureType2DArray:
			rtInfo.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtInfo.Texture2DArray.MipSlice = 0;
			rtInfo.Texture2DArray.FirstArraySlice = 0;
			rtInfo.Texture2DArray.ArraySize = 0;
			break;
		case core::gfx::tex::kTextureType3D:
			rtInfo.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			rtInfo.Texture3D.MipSlice = 0;
			break;
		default:
			rtInfo.ViewDimension = D3D11_RTV_DIMENSION_UNKNOWN;
			break;
		}

		result = device->CreateRenderTargetView((ID3D11Resource*)texture->nativePtr(), &rtInfo, (ID3D11RenderTargetView**)&m_attachments[slot]);
		if (FAILED(result)) {
			m_hasFailure = true;
			return kErrorFormatUnsupported;
		}
	}
	else
	{
		return kErrorBadArgument;
	}

	return 0;
}

int gpu::RenderTarget::attach ( int slot, WOFrameAttachment* buffer )
{
	ID3D11Device*	device = gpu::getDevice()->getNative();
	HRESULT			result;

	if (buffer->m_texture == NULL)
	{
		m_hasFailure = true;
		return kErrorBadArgument;
	}

	if (slot == kRenderTargetSlotDepthStencil)
	{
		ARCORE_ASSERT(m_attachmentDepthStencil == NULL);
		m_highlevelAttachments[15 - slot] = buffer;

		D3D11_DEPTH_STENCIL_VIEW_DESC rtInfo = {};
		rtInfo.Format = gpu::internal::ArEnumToDx(buffer->m_format, true, false);
		rtInfo.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		rtInfo.Texture2D.MipSlice = 0;

		result = device->CreateDepthStencilView((ID3D11Resource*)buffer->nativePtr(), &rtInfo, (ID3D11DepthStencilView**)&m_attachmentDepthStencil);
		if (FAILED(result)) {
			m_hasFailure = true;
			return kErrorFormatUnsupported;
		}
	}
	else if (slot >= kRenderTargetSlotColor0)
	{
		ARCORE_ASSERT(m_attachments[slot] == NULL);
		m_highlevelAttachments[slot] = buffer;

		D3D11_RENDER_TARGET_VIEW_DESC rtInfo = {};
		rtInfo.Format = gpu::internal::ArEnumToDx(buffer->m_format, true, false);
		rtInfo.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtInfo.Texture2D.MipSlice = 0;

		result = device->CreateRenderTargetView((ID3D11Resource*)buffer->nativePtr(), &rtInfo, (ID3D11RenderTargetView**)&m_attachments[slot]);
		if (FAILED(result)) {
			m_hasFailure = true;
			return kErrorFormatUnsupported;
		}
	}
	else 
	{
		return kErrorBadArgument;
	}

	return kError_SUCCESS;
}

int gpu::RenderTarget::assemble ( void )
{
	if (m_hasFailure)
	{
		m_assembled = false;
		return kErrorCreationFailed;
	}

	m_assembled = true;
	return kError_SUCCESS;
}

bool gpu::RenderTarget::empty ( void )
{
	// TODO: Should check the attached stuff to make sure it's not empty
	return !m_assembled;
}
bool gpu::RenderTarget::valid ( void )
{
	return !m_hasFailure && m_assembled;
}

gpuHandle gpu::RenderTarget::nativePtr ( void )
{
	return 0;
}

gpu::Texture* gpu::RenderTarget::getAttachment ( int slot )
{
	if (slot >= 0)
		return (gpu::Texture*)m_highlevelAttachments[slot];
	else
		return (gpu::Texture*)m_highlevelAttachments[15 - slot];
}

gpu::WOFrameAttachment* gpu::RenderTarget::getWOAttachment ( int slot )
{
	if (slot >= 0)
		return (gpu::WOFrameAttachment*)m_highlevelAttachments[slot];
	else
		return (gpu::WOFrameAttachment*)m_highlevelAttachments[15 - slot];
}

#endif