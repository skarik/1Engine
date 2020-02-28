#ifndef GPU_WRAPPER_INTERNAL_ENUMS_H_
#define GPU_WRAPPER_INTERNAL_ENUMS_H_

#include "core/types/types.h"
#include "core/exceptions.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"
#include "gpuw/Public/Enums.h"
#include "gpuw/Public/Formats.h"
#include "gpuw/Public/ShaderTypes.h"
#include "gpuw/Public/PipelineCreationParams.h"
#include "gpuw/Device.h"

namespace gpu {
namespace internal {

	static FORCE_INLINE DXGI_FORMAT ArEnumToDx ( const core::gfx::tex::arColorFormat format )
	{
		using namespace core::gfx::tex;
		switch ( format )
		{
			// Color formats:
		case kColorFormatRGB8:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		case kColorFormatRGB16:		return DXGI_FORMAT_R16G16B16A16_UNORM;
		case kColorFormatRGB16F:	return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case kColorFormatRGB32:		return DXGI_FORMAT_R32G32B32A32_UINT;
		case kColorFormatRGB32F:	return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case kColorFormatRGBA8:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		case kColorFormatRGBA16:	return DXGI_FORMAT_R16G16B16A16_UNORM;
		case kColorFormatRGBA16F:	return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case kColorFormatRGBA32:	return DXGI_FORMAT_R32G32B32A32_UINT;
		case kColorFormatRGBA32F:	return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case kColorFormatRG8:		return DXGI_FORMAT_R8G8_UNORM;
		case kColorFormatRG16:		return DXGI_FORMAT_R16G16_UNORM;
		case kColorFormatRG16F:		return DXGI_FORMAT_R16G16_FLOAT;
		case kColorFormatRG32:		return DXGI_FORMAT_R32G32_UINT;
		case kColorFormatRG32F:		return DXGI_FORMAT_R32G32_FLOAT;

		case kColorFormatR8:		return DXGI_FORMAT_R8_UNORM;
		case kColorFormatR16:		return DXGI_FORMAT_R16_UNORM;
		case kColorFormatR16F:		return DXGI_FORMAT_R16_FLOAT;
		case kColorFormatR32:		return DXGI_FORMAT_R32_UINT;
		case kColorFormatR32F:		return DXGI_FORMAT_R32_FLOAT;

			// Depth formats:
		case kDepthFormat16:		return DXGI_FORMAT_D16_UNORM;
			//case kDepthFormat24:		return DXGI_FORMAT_D24_UNORM;
		case kDepthFormat32:		return DXGI_FORMAT_D32_FLOAT;
		case kDepthFormat32F:		return DXGI_FORMAT_D32_FLOAT;

			// Stencil formats:
		case KStencilFormatIndex1:	return DXGI_FORMAT_R1_UNORM;
			//case KStencilFormatIndex4:	return DXGI_FORMAT_R4_UNORM;
		case KStencilFormatIndex8:	return DXGI_FORMAT_R8_UNORM;
		case KStencilFormatIndex16:	return DXGI_FORMAT_R16_UNORM;

			// Packed Depth+Stencil formats:
		case kDepthFormat32FStencil8:	return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case kDepthFormat24Stencil8:	return DXGI_FORMAT_D24_UNORM_S8_UINT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	static FORCE_INLINE DXGI_FORMAT ArEnumToDx ( const gpu::Format format )
	{
		switch ( format ) {
			// Byte formats
		case kFormatR8UInteger:				return DXGI_FORMAT_R8_UINT;
		case kFormatR8G8UInteger:			return DXGI_FORMAT_R8G8_UINT;
		case kFormatR8G8B8UInteger:			return DXGI_FORMAT_UNKNOWN;
		case kFormatR8G8B8A8UInteger:		return DXGI_FORMAT_R8G8B8A8_UINT;
			// Short formats
		case kFormatR16UInteger:			return DXGI_FORMAT_R16_UINT;
		case kFormatR16G16UInteger:			return DXGI_FORMAT_R16G16_UINT;
		case kFormatR16G16B16UInteger:		return DXGI_FORMAT_UNKNOWN;
		case kFormatR16G16B16A16UInteger:	return DXGI_FORMAT_R16G16B16A16_UINT;
			// Int formats
		case kFormatR32UInteger:			return DXGI_FORMAT_R32_UINT;
			// Float formats
		case kFormatR32SFloat:				return DXGI_FORMAT_R32_FLOAT;
		case kFormatR32G32SFloat:			return DXGI_FORMAT_R32G32_FLOAT;
		case kFormatR32G32B32SFloat:		return DXGI_FORMAT_R32G32B32_FLOAT;
		case kFormatR32G32B32A32SFloat:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		throw core::InvalidArgumentException();
		return DXGI_FORMAT_UNKNOWN;
	}

	static FORCE_INLINE D3D11_INPUT_CLASSIFICATION ArEnumToDx ( const gpu::InputRate format )
	{
		switch ( format ) {
			// Byte formats
		case kInputRatePerVertex:		return D3D11_INPUT_PER_VERTEX_DATA;
		case kInputRatePerInstance:		return D3D11_INPUT_PER_INSTANCE_DATA;
		}
		throw core::InvalidArgumentException();
		return D3D11_INPUT_PER_VERTEX_DATA;
	}

	static FORCE_INLINE D3D11_PRIMITIVE_TOPOLOGY ArEnumToDx ( const gpu::PrimitiveTopology format )
	{
		switch ( format ) {
			// Byte formats
		case kPrimitiveTopologyPointList:		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case kPrimitiveTopologyLineList:		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case kPrimitiveTopologyLineStrip:		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case kPrimitiveTopologyTriangleList:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case kPrimitiveTopologyTriangleStrip:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case kPrimitiveTopologyTriangleFan:		return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case kPrimitiveTopologyLineListAdjacency:	return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
		case kPrimitiveTopologyLineStripAdjacency:	return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
		case kPrimitiveTopologyTriangleListAdjacency:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
		case kPrimitiveTopologyTriangleStripAdjacency:	return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
		case kPrimitiveTopologyLinePatchList:	return D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
		}
		throw core::InvalidArgumentException();
		return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
	
}}

#endif//GPU_WRAPPER_INTERNAL_ENUMS_H_
