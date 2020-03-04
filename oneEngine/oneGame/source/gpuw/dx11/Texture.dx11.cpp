#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./Device.dx11.h"
#include "./Texture.dx11.h"
#include "./Buffers.dx11.h"
#include "./Internal/Enums.dx11.h"
#include "gpuw/Public/Error.h"
#include "core/exceptions.h"
#include "core/math/Math.h"
#include <algorithm>

//https://computergraphics.stackexchange.com/questions/4422/directx-openglvulkan-concepts-mapping-chart/4434

bool gpu::Texture::valid ( void )
{
	return m_texture != 0;
}
gpuHandle gpu::Texture::nativePtr ( void )
{
	return (gpuHandle)m_texture;
}

int gpu::Texture::allocate (
	const core::gfx::tex::arTextureType textureType,
	const core::gfx::tex::arColorFormat textureFormat, 
	const uint width, const uint height, const uint depth, const uint levels
)
{
	ID3D11Device*	device = gpu::getDevice()->getNative();
	HRESULT			result;

	uint allocatedLevels = levels;
	if (allocatedLevels == 0)
	{
		// Generate correct number of mipmaps down to 1x1:
		allocatedLevels = (uint16_t) std::max<int>( 1, math::log2( std::max<uint>(width, std::max<uint>(height, depth)) ) + 1 );
	}

	if (m_texture == NULL)
	{
		m_type = textureType;
		m_format = textureFormat;
		m_dxFormat = gpu::internal::ArEnumToDx(textureFormat, false, true);

		// Allocate storage
		switch (m_type)
		{
		case core::gfx::tex::kTextureType2D:
		case core::gfx::tex::kTextureType2DArray:
		case core::gfx::tex::kTextureTypeCube:
		case core::gfx::tex::kTextureTypeCubeArray:
			m_width = width;
			m_height = height;
			m_depth = ((m_type == core::gfx::tex::kTextureTypeCube) ? 6 : 1) * std::max<uint>(1, depth);
			m_levels = allocatedLevels;
			{
				D3D11_TEXTURE2D_DESC		txd;
				txd.Width = width;
				txd.Height = height;
				txd.MipLevels = allocatedLevels;
				txd.ArraySize = m_depth;
				txd.Format = gpu::internal::ArEnumToDx(textureFormat, false, true);
				txd.SampleDesc.Count = 1;
				txd.SampleDesc.Quality = 0;
				txd.Usage = D3D11_USAGE_DEFAULT;
				txd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				txd.CPUAccessFlags = 0;
				txd.MiscFlags = 0;

				if (textureFormat == core::gfx::tex::kDepthFormat16
					|| textureFormat == core::gfx::tex::kDepthFormat24
					|| textureFormat == core::gfx::tex::kDepthFormat24Stencil8
					|| textureFormat == core::gfx::tex::kDepthFormat32
					|| textureFormat == core::gfx::tex::kDepthFormat32F
					|| textureFormat == core::gfx::tex::kDepthFormat32FStencil8
					|| textureFormat == core::gfx::tex::KStencilFormatIndex1
					|| textureFormat == core::gfx::tex::KStencilFormatIndex4
					|| textureFormat == core::gfx::tex::KStencilFormatIndex8
					|| textureFormat == core::gfx::tex::KStencilFormatIndex16)
				{
					txd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
				}

				result = device->CreateTexture2D(&txd, NULL, (ID3D11Texture2D**)&m_texture);
				if (FAILED(result))
				{
					//throw core::OutOfMemoryException(); // TODO: Handle this better.
					m_texture = NULL;
					return gpu::kErrorFormatUnsupported;
				}

				D3D11_SHADER_RESOURCE_VIEW_DESC srvInfo;
				srvInfo.Format = gpu::internal::ArEnumToDx(textureFormat, false, false);
				if (m_type == core::gfx::tex::kTextureType2D)
				{
					srvInfo.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvInfo.Texture2D.MostDetailedMip = 0;
					srvInfo.Texture2D.MipLevels = txd.MipLevels;
				}
				else if (m_type == core::gfx::tex::kTextureType2DArray)
				{
					srvInfo.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					srvInfo.Texture2DArray.MostDetailedMip = 0;
					srvInfo.Texture2DArray.MipLevels = txd.MipLevels;
					srvInfo.Texture2DArray.FirstArraySlice = 0;
					srvInfo.Texture2DArray.ArraySize = txd.ArraySize;
				}
				else if (m_type == core::gfx::tex::kTextureTypeCube)
				{
					srvInfo.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					srvInfo.TextureCube.MostDetailedMip = 0;
					srvInfo.TextureCube.MipLevels = txd.MipLevels;
				}
				else if (m_type == core::gfx::tex::kTextureTypeCubeArray)
				{
					srvInfo.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					srvInfo.TextureCubeArray.MostDetailedMip = 0;
					srvInfo.TextureCubeArray.MipLevels = txd.MipLevels;
					srvInfo.TextureCubeArray.First2DArrayFace = 0;
					srvInfo.TextureCubeArray.NumCubes = txd.ArraySize / 6;
				}
				
				result = device->CreateShaderResourceView((ID3D11Texture2D*)m_texture, &srvInfo, (ID3D11ShaderResourceView**)&m_srv);
				if (FAILED(result))
					throw core::OutOfMemoryException(); // TODO: Handle this better.
			}
			break;
		case core::gfx::tex::kTextureType1D:
		case core::gfx::tex::kTextureType1DArray:
			m_width = width;
			m_height = std::max<uint>(1, height);
			m_depth = 1;
			m_levels = allocatedLevels;
			{
				D3D11_TEXTURE1D_DESC		txd;
				txd.Width = width;
				txd.MipLevels = allocatedLevels;
				txd.ArraySize = m_height;
				txd.Format = gpu::internal::ArEnumToDx(textureFormat, false, true);
				txd.Usage = D3D11_USAGE_DEFAULT;
				txd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				txd.CPUAccessFlags = 0;
				txd.MiscFlags = 0;

				result = device->CreateTexture1D(&txd, NULL, (ID3D11Texture1D**)&m_texture);
				if (FAILED(result))
				{
					//throw core::OutOfMemoryException(); // TODO: Handle this better.
					m_texture = NULL;
					return gpu::kErrorFormatUnsupported;
				}
			}
			break;
		case core::gfx::tex::kTextureType3D:
			m_width = width;
			m_height = height;
			m_depth = depth;
			m_levels = allocatedLevels;
			{
				D3D11_TEXTURE3D_DESC		txd;
				txd.Width = width;
				txd.Height = height;
				txd.Depth = depth;
				txd.MipLevels = allocatedLevels;
				txd.Format = gpu::internal::ArEnumToDx(textureFormat, false, true);
				txd.Usage = D3D11_USAGE_DEFAULT;
				txd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				txd.CPUAccessFlags = 0;
				txd.MiscFlags = 0;

				result = device->CreateTexture3D(&txd, NULL, (ID3D11Texture3D**)&m_texture);
				if (FAILED(result))
				{
					//throw core::OutOfMemoryException(); // TODO: Handle this better.
					m_texture = NULL;
					return gpu::kErrorFormatUnsupported;
				}

				D3D11_SHADER_RESOURCE_VIEW_DESC srvInfo;
				srvInfo.Format = gpu::internal::ArEnumToDx(textureFormat, false, false);
				if (m_type == core::gfx::tex::kTextureType3D)
				{
					srvInfo.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
					srvInfo.Texture3D.MostDetailedMip = 0;
					srvInfo.Texture3D.MipLevels = txd.MipLevels;
				}
				
				result = device->CreateShaderResourceView((ID3D11Texture3D*)m_texture, &srvInfo, (ID3D11ShaderResourceView**)&m_srv);
				if (FAILED(result))
					throw core::OutOfMemoryException(); // TODO: Handle this better.
			}
			break;
		}
	}
	return gpu::kError_SUCCESS;
}

//	free() : destroys any allocated texture, if existing.
int gpu::Texture::free ( void )
{
	if (m_texture)
		static_cast<ID3D11Resource*>(m_texture)->Release();
	if (m_srv)
		static_cast<ID3D11ShaderResourceView*>(m_srv)->Release();
	
	m_texture = NULL;
	m_srv = NULL;

	return gpu::kError_SUCCESS;
}

int gpu::Texture::upload ( gpu::Buffer& buffer, const uint level )
{
	/*const uint level_divisor = (1 << level);
	GLsizei upload_width, upload_height;

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, (GLuint)buffer.nativePtr());
	switch (m_type)
	{
	case core::gfx::tex::kTextureType1D:
		ARCORE_ERROR("Not implemented");
		break;
	case core::gfx::tex::kTextureType1DArray:
	case core::gfx::tex::kTextureType2D:
	case core::gfx::tex::kTextureTypeCube:
		upload_width  = std::max<GLsizei>(1, m_width / level_divisor);
		upload_height = std::max<GLsizei>(1, m_height / level_divisor);
		glTextureSubImage2D(m_texture, level, 0, 0, upload_width, upload_height, m_glcomponent, m_gltype, NULL);
		break;
	case core::gfx::tex::kTextureType2DArray:
	case core::gfx::tex::kTextureType3D:
	case core::gfx::tex::kTextureTypeCubeArray:
		ARCORE_ERROR("Not implemented");
		break;
	}
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// Lot of things can go wrong with textures, and this is a slow process here. It doesn't hurt to check status here.
	ARCORE_ASSERT(glGetError() == GLenum(0));*/

	ID3D11Device*			device = gpu::getDevice()->getNative();
	ID3D11DeviceContext*	ctx = gpu::getDevice()->getNativeContext();
	ID3D11Texture2D*		texture = (ID3D11Texture2D*)m_texture;
	D3D11_BOX				src_box;
	ID3D11Buffer*			src_buffer = (ID3D11Buffer*)buffer.nativePtr();

	const uint				level_divisor = (1 << level);
	src_box.left = 0;
	src_box.top = 0;
	src_box.front = 0;
	src_box.right = std::max<UINT>(1, m_width / level_divisor);
	src_box.bottom = std::max<UINT>(1, m_height / level_divisor);
	src_box.back = 1;

	ctx->CopySubresourceRegion(texture,
							   D3D11CalcSubresource(level, 0, m_levels),
							   0, 0, 0,
							   src_buffer,
							   0,
							   &src_box);

	return gpu::kError_SUCCESS;
}


bool gpu::WOFrameAttachment::valid ( void )
{
	return m_texture != 0;
}
gpuHandle gpu::WOFrameAttachment::nativePtr ( void )
{
	return (gpuHandle)m_texture;
}

int gpu::WOFrameAttachment::allocate (
	const core::gfx::tex::arTextureType textureType,
	const core::gfx::tex::arColorFormat textureFormat, 
	const uint width, const uint height, const uint depth, const uint levels
)
{
	ID3D11Device*	device = gpu::getDevice()->getNative();
	HRESULT			result;

	if (textureType != core::gfx::tex::kTextureType2D)
	{
		throw core::InvalidArgumentException(); // TODO: Handle this better.
		return gpu::kErrorBadArgument;
	}

	if (m_texture == 0)
	{
		m_type = textureType;
		m_format = textureFormat;
		m_dxFormat = gpu::internal::ArEnumToDx(textureFormat, false, false);
		/*// Create texture
		glCreateRenderbuffers(1, &m_texture);
		if (m_texture != 0)
		{
			// Allocate storage:
			glNamedRenderbufferStorage(m_texture, gpu::internal::ArEnumToGL(textureFormat), width, height);
		}
		else
		{
			throw core::OutOfMemoryException(); // TODO: Handle this better.
			return gpu::kErrorOutOfMemory;
		}*/
		//m_width = width;
		//m_height = height;
		//m_depth = ((m_type == core::gfx::tex::kTextureTypeCube) ? 6 : 1) * std::max<uint>(1, depth);
		//m_levels = allocatedLevels;
		{
			D3D11_TEXTURE2D_DESC		txd;
			txd.Width = width;
			txd.Height = height;
			txd.MipLevels = 1;
			txd.ArraySize = 1;
			txd.Format = gpu::internal::ArEnumToDx(textureFormat, false, false);
			txd.SampleDesc.Count = 1;
			txd.SampleDesc.Quality = 0;
			txd.Usage = D3D11_USAGE_DEFAULT;
			txd.BindFlags = D3D11_BIND_RENDER_TARGET;
			if (textureFormat == core::gfx::tex::kDepthFormat16
				|| textureFormat == core::gfx::tex::kDepthFormat24
				|| textureFormat == core::gfx::tex::kDepthFormat24Stencil8
				|| textureFormat == core::gfx::tex::kDepthFormat32
				|| textureFormat == core::gfx::tex::kDepthFormat32F
				|| textureFormat == core::gfx::tex::kDepthFormat32FStencil8
				|| textureFormat == core::gfx::tex::KStencilFormatIndex1
				|| textureFormat == core::gfx::tex::KStencilFormatIndex4
				|| textureFormat == core::gfx::tex::KStencilFormatIndex8
				|| textureFormat == core::gfx::tex::KStencilFormatIndex16)
			{
				txd.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_DEPTH_STENCIL;
			}
			txd.CPUAccessFlags = 0;
			txd.MiscFlags = 0;

			result = device->CreateTexture2D(&txd, NULL, (ID3D11Texture2D**)&m_texture);
			if (FAILED(result))
			{
				//throw core::OutOfMemoryException(); // TODO: Handle this better.
				m_texture = NULL;
				return gpu::kErrorOutOfMemory;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvInfo;
			srvInfo.Format = gpu::internal::ArEnumToDx(textureFormat, false, true);
			if (m_type == core::gfx::tex::kTextureType2D)
			{
				srvInfo.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvInfo.Texture2D.MostDetailedMip = 0;
				srvInfo.Texture2D.MipLevels = txd.MipLevels;
			}

			result = device->CreateShaderResourceView((ID3D11Texture3D*)m_texture, &srvInfo, (ID3D11ShaderResourceView**)&m_srv);
			if (FAILED(result))
				throw core::OutOfMemoryException(); // TODO: Handle this better.
		}

	}
	return gpu::kError_SUCCESS;
}

//	free() : destroys any allocated texture, if existing.
int gpu::WOFrameAttachment::free ( void )
{
	if (m_texture)
		static_cast<ID3D11Resource*>(m_texture)->Release();
	if (m_srv)
		static_cast<ID3D11ShaderResourceView*>(m_srv)->Release();

	m_texture = NULL;
	m_srv = NULL;

	return gpu::kError_SUCCESS;
}

#endif