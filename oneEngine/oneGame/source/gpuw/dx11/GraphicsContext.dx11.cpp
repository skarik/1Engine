#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./GraphicsContext.dx11.h"
#include "./Device.dx11.h"
#include "./Pipeline.dx11.h"
#include "./ShaderPipeline.dx11.h"
#include "./Sampler.dx11.h"
#include "./Buffers.dx11.h"
#include "./Fence.dx11.h"
#include "./RenderTarget.dx11.h"
#include "./Internal/Enums.dx11.h"
#include "gpuw/Public/Error.h"

#include "core/debug.h"
#include "core/exceptions.h"

#include <stdio.h>
#include <map>
#include <codecvt>

//===============================================================================================//

struct GPUBlendStateFilter
{
	uint64_t filter[4];

	friend bool operator< (const GPUBlendStateFilter& l, const GPUBlendStateFilter& r)
	{
		return l.filter[0] < r.filter[0]
			|| l.filter[1] < r.filter[1]
			|| l.filter[2] < r.filter[2]
			|| l.filter[3] < r.filter[3];
	}
};

typedef std::map<uint32_t, ID3D11RasterizerState*> GPURasterizerStateMap;
typedef std::map<GPUBlendStateFilter, ID3D11BlendState*> GPUBlendStateMap;
typedef std::map<uint64_t, ID3D11DepthStencilState*> GPUDepthStencilStateMap;

//===============================================================================================//

static FORCE_INLINE D3D11_FILL_MODE ArEnumToDx ( const gpu::FillMode mode )
{
	using namespace gpu;
	switch (mode)
	{
	case kFillModeWireframe:	return D3D11_FILL_WIREFRAME;
	case kFillModeSolid:		return D3D11_FILL_SOLID;
	}
	throw core::InvalidArgumentException();
	return D3D11_FILL_SOLID;
}

static FORCE_INLINE D3D11_CULL_MODE ArEnumToDx ( const gpu::CullMode mode )
{
	using namespace gpu;
	switch (mode)
	{
	case kCullModeNone:		return D3D11_CULL_NONE;
	case kCullModeFront:	return D3D11_CULL_FRONT;
	case kCullModeBack:		return D3D11_CULL_BACK;
	}
	throw core::InvalidArgumentException();
	return D3D11_CULL_NONE;
}

static FORCE_INLINE D3D11_BLEND ArEnumToDx ( const gpu::BlendMode mode )
{
	using namespace gpu;
	switch (mode)
	{
	case kBlendModeZero:		return D3D11_BLEND_ZERO;
	case kBlendModeOne:			return D3D11_BLEND_ONE;
	case kBlendModeSrcColor:	return D3D11_BLEND_SRC_COLOR;
	case kBlendModeInvSrcColor:	return D3D11_BLEND_INV_SRC_COLOR;
	case kBlendModeSrcAlpha:	return D3D11_BLEND_SRC_ALPHA;
	case kBlendModeInvSrcAlpha:	return D3D11_BLEND_INV_SRC_ALPHA;
	case kBlendModeDstColor:	return D3D11_BLEND_DEST_COLOR;
	case kBlendModeInvDstColor:	return D3D11_BLEND_INV_DEST_COLOR;
	case kBlendModeDstAlpha:	return D3D11_BLEND_DEST_ALPHA;
	case kBlendModeInvDstAlpha:	return D3D11_BLEND_INV_DEST_ALPHA;
	case kBlendModeSrcAlphaSat:	return D3D11_BLEND_SRC_ALPHA_SAT;
	case kBlendModeBlendFactor:	return D3D11_BLEND_BLEND_FACTOR;
	case kBlendModeInvBlendFactor:	return D3D11_BLEND_INV_BLEND_FACTOR;
	case kBlendModeSrc1Color:	return D3D11_BLEND_SRC1_COLOR;
	case kBlendModeInvSrc1Color:	return D3D11_BLEND_INV_SRC1_COLOR;
	case kBlendModeSrc1Alpha:	return D3D11_BLEND_SRC1_ALPHA;
	case kBlendModeInvSrc1Alpha:	return D3D11_BLEND_INV_SRC1_ALPHA;
	}
	throw core::InvalidArgumentException();
	return D3D11_BLEND_ZERO;
}

static FORCE_INLINE D3D11_BLEND_OP ArEnumToDx ( const gpu::BlendOp mode )
{
	using namespace gpu;
	switch (mode)
	{
	case kBlendOpAdd:		return D3D11_BLEND_OP_ADD;
	case kBlendOpSubtract:	return D3D11_BLEND_OP_SUBTRACT;
	case kBlendOpRevSubtract:	return D3D11_BLEND_OP_REV_SUBTRACT;
	case kBlendOpMin:		return D3D11_BLEND_OP_MIN;
	case kBlendOpMax:		return D3D11_BLEND_OP_MAX;
	}
	throw core::InvalidArgumentException();
	return D3D11_BLEND_OP_ADD;
}

static FORCE_INLINE D3D11_COMPARISON_FUNC ArEnumToDx ( const gpu::CompareOp mode )
{
	using namespace gpu;
	switch (mode)
	{
	case kCompareOpNever:		return D3D11_COMPARISON_NEVER;
	case kCompareOpLess:		return D3D11_COMPARISON_LESS;
	case kCompareOpEqual:		return D3D11_COMPARISON_EQUAL;
	case kCompareOpLessEqual:	return D3D11_COMPARISON_LESS_EQUAL;
	case kCompareOpGreater:		return D3D11_COMPARISON_GREATER;
	case kCompareOpNotEqual:	return D3D11_COMPARISON_NOT_EQUAL;
	case kCompareOpGreaterEqual:	return D3D11_COMPARISON_GREATER_EQUAL;
	case kCompareOpAlways:		return D3D11_COMPARISON_ALWAYS;
	}
	throw core::InvalidArgumentException();
	return D3D11_COMPARISON_NEVER;
}

static FORCE_INLINE D3D11_STENCIL_OP ArEnumToDx ( const gpu::StencilOp mode )
{
	using namespace gpu;
	switch (mode)
	{
	case kStencilOpKeep:		return D3D11_STENCIL_OP_KEEP;
	case kStencilOpZero:		return D3D11_STENCIL_OP_ZERO;
	case kStencilOpReplace:		return D3D11_STENCIL_OP_REPLACE;
	case kStencilOpIncrementSaturate:	return D3D11_STENCIL_OP_INCR_SAT;
	case kStencilOpDecrementSaturate:	return D3D11_STENCIL_OP_DECR_SAT;
	case kStencilOpInvert:		return D3D11_STENCIL_OP_INVERT;
	case kStencilOpIncrement:	return D3D11_STENCIL_OP_INCR;
	case kStencilOpDecrement:	return D3D11_STENCIL_OP_DECR;
	}
	throw core::InvalidArgumentException();
	return D3D11_STENCIL_OP_KEEP;
}

//===============================================================================================//

gpu::GraphicsContext::GraphicsContext ( Device* wrapperDevice )
	: m_rasterStateCachedMap(NULL), m_rasterStateCurrentBitfilter((uint32_t)-1),
	m_blendStateCachedMap(NULL), m_blendStateCurrentBitfilter(),
	m_depthStateCachedMap(NULL), m_depthStateCurrentBitfilter((uint64_t)-1)
{
	//SamplerCreationDescription scd = SamplerCreationDescription();
	//m_defaultSampler = new Sampler;
	//m_defaultSampler->create(NULL, &scd);

	HRESULT			result;
	gpu::Device*	gpuDevice = (gpu::Device*)wrapperDevice;

	// Save the device for submit
	m_wrapperDevice = wrapperDevice;

	// Grab a new context
	result = gpuDevice->getNative()->CreateDeferredContext(0, (ID3D11DeviceContext**)&m_deferredContext);
	if (FAILED(result))
	{
		// TODO: Error handling. Is a bitch to do in the constructor.
		abort();
	}

	// Create the needed cached states
	m_rasterStateCachedMap = new GPURasterizerStateMap;
	m_blendStateCachedMap = new GPUBlendStateMap;
	m_blendStateCurrentBitfilter[0] = (uint64_t)(-1);
	m_depthStateCachedMap = new GPUDepthStencilStateMap;

	// Create default sampler
	SamplerCreationDescription scd = SamplerCreationDescription();
	m_defaultSampler = new Sampler;
	m_defaultSampler->create(NULL, &scd);
}

gpu::GraphicsContext::~GraphicsContext ( void )
{
	// Free up all cached rasterizer states
	GPURasterizerStateMap*	rsMap = (GPURasterizerStateMap*)m_rasterStateCachedMap;
	for (auto rsPair : *rsMap)
	{
		rsPair.second->Release();
	}
	delete rsMap;
	m_rasterStateCachedMap = NULL;

	// Free up all cached blend states
	GPUBlendStateMap*		bsMap = (GPUBlendStateMap*)m_blendStateCachedMap;
	for (auto bsPair : *bsMap)
	{
		bsPair.second->Release();
	}
	delete bsMap;
	m_blendStateCachedMap = NULL;

	// Free sampler
	m_defaultSampler->destroy(NULL);
	delete m_defaultSampler;

	// Done w/ context
	static_cast<ID3D11DeviceContext*>(m_deferredContext)->Release();
}

int gpu::GraphicsContext::reset ( void )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	ctx->ClearState(); // TODO: Remove this line, and investigate if it causes issues, if any.

	m_rasterStateCurrentBitfilter = (uint32_t)-1;
	m_blendStateCurrentBitfilter[0] = (uint64_t)-1;
	m_blendStateCurrentBitfilter[1] = (uint64_t)-1;
	m_blendStateCurrentBitfilter[2] = (uint64_t)-1;
	m_blendStateCurrentBitfilter[3] = (uint64_t)-1;
	m_depthStateCurrentBitfilter = (uint64_t)-1;;

	return kError_SUCCESS;
}

int gpu::GraphicsContext::submit ( void )
{
	gpu::Device*			gpuDevice = (gpu::Device*)m_wrapperDevice;
	ID3D11CommandList*		commandList = NULL;
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	HRESULT					result;

	result = ctx->FinishCommandList(FALSE, &commandList); // Finalize all commands thrown in.
	if (FAILED(result))
	{
		throw core::InvalidCallException();
	}

	gpuDevice->getImmediateContext()->ExecuteCommandList(commandList, FALSE);

	commandList->Release();

	return kError_SUCCESS;
}

int gpu::GraphicsContext::validate ( void )
{
	return kError_SUCCESS;
}

//===============================================================================================//

int gpu::GraphicsContext::setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	D3D11_VIEWPORT			viewport;
	viewport.TopLeftX	= (float)left;
	viewport.TopLeftY	= (float)top;
	viewport.Width		= (float)(right - left);
	viewport.Height		= (float)(bottom - top);
	viewport.MinDepth	= 0.0F;
	viewport.MaxDepth	= 1.0F;

	ctx->RSSetViewports(1, &viewport);

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;

	D3D11_RECT				scissor;
	scissor.left	= left;
	scissor.top		= top;
	scissor.right	= right;
	scissor.bottom	= bottom;

	ctx->RSSetScissorRects(1, &scissor);

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setRasterizerState ( const RasterizerState& state )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	ID3D11Device*			device = static_cast<gpu::Device*>(m_wrapperDevice)->getNative();
	GPURasterizerStateMap*	rsMap = (GPURasterizerStateMap*)m_rasterStateCachedMap;
	uint32_t				bitFilter;

	D3D11_RASTERIZER_DESC	rsDesc = {};
	rsDesc.FillMode				= ArEnumToDx(state.fillmode);
	rsDesc.CullMode				= ArEnumToDx(state.cullmode);
	rsDesc.FrontCounterClockwise	= (state.frontface == kFrontFaceCounterClockwise) ? TRUE : FALSE;
	rsDesc.DepthBias			= 0;
	rsDesc.DepthBiasClamp		= 0.0F;
	rsDesc.SlopeScaledDepthBias	= 0.0F;
	rsDesc.DepthClipEnable		= TRUE;
	rsDesc.ScissorEnable		= state.scissorEnabled ? TRUE : FALSE;
	rsDesc.MultisampleEnable	= FALSE;
	rsDesc.AntialiasedLineEnable	= FALSE;

	// Build filter used for fast lookup of existing states
	bitFilter = (rsDesc.FillMode << 0)
				| (rsDesc.CullMode << 2)
				| (rsDesc.FrontCounterClockwise << 3)
				| (rsDesc.ScissorEnable << 4);

	// Filter out uneccessary calls
	if (bitFilter != m_rasterStateCurrentBitfilter) // TODO: Check if this filtering actually provides any speed increase, remove if necessary
	{
		// Locate the bitfilter in the cached map
		ID3D11RasterizerState*	rs;
		auto rsPair = rsMap->find(bitFilter);

		if (rsPair == rsMap->end())
		{
			HRESULT result = device->CreateRasterizerState(&rsDesc, &rs);
			if (FAILED(result))
				throw core::NullReferenceException();
			(*rsMap)[bitFilter] = rs;
		}
		else
			rs = rsPair->second;

		// Set the updated raster state
		m_rasterStateCurrentBitfilter = bitFilter;
		ctx->RSSetState(rs);
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setBlendState ( const BlendState& state )
{	
	BlendCollectiveState	bcs = {};
	bcs.blend[0] = state;
	this->setBlendCollectiveState(bcs);

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setBlendCollectiveState ( const BlendCollectiveState& state )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	ID3D11Device*			device = static_cast<gpu::Device*>(m_wrapperDevice)->getNative();
	uint64_t				bitFilter [4] = {0, 0, 0, 0};
	GPUBlendStateMap*		bsMap = (GPUBlendStateMap*)m_blendStateCachedMap;

	// TODO: DX11 Api may do all this automatically (as per documentation) so maybe uneccesary to filter.

	// There is no easy filtering this state, so we'll just cache all the variants and compare the values manually.

	// Filter size for the DX11 blend state is 8 uint32s
	for (uint32_t i = 0; i < 8; ++i)
	{
		uint64_t subFilter = (state.blend[i].enable ? 1 : 0)
							| ((state.blend[i].src & 0x1F) << 1) // 5
							| ((state.blend[i].dst & 0x1F) << 6) // 5
							| ((state.blend[i].op & 0x07) << 11) // 3
							| ((state.blend[i].srcAlpha & 0x1F) << 14) // 5
							| ((state.blend[i].dstAlpha & 0x1F) << 19) // 5
							| ((state.blend[i].opAlpha & 0x07) << 24) // 3
							| ((state.blend[i].channelMask & 0x0F) << 27); // 4
		bitFilter[i / 2] |= subFilter << ((i % 2) * 32);
	}

	// Filter out uncessary calls
	if (bitFilter[0] != m_blendStateCurrentBitfilter[0]
		|| bitFilter[1] != m_blendStateCurrentBitfilter[1]
		|| bitFilter[2] != m_blendStateCurrentBitfilter[2]
		|| bitFilter[3] != m_blendStateCurrentBitfilter[3])
	{
		// Locate the bitfilter in the cached map
		ID3D11BlendState*	bs;
		GPUBlendStateFilter bitFilerAssembled = {bitFilter[0], bitFilter[1], bitFilter[2], bitFilter[3]};
		auto bsPair = bsMap->find(bitFilerAssembled);

		if (bsPair == bsMap->end())
		{
			D3D11_BLEND_DESC	bsDesc = {};
			bsDesc.AlphaToCoverageEnable	= false;
			bsDesc.IndependentBlendEnable	= false;
			for (uint32_t i = 0; i < 8; ++i)
			{
				bsDesc.RenderTarget[i].BlendEnable	= state.blend[i].enable ? TRUE : FALSE;
				bsDesc.RenderTarget[i].SrcBlend		= ArEnumToDx(state.blend[i].src);
				bsDesc.RenderTarget[i].DestBlend	= ArEnumToDx(state.blend[i].dst);
				bsDesc.RenderTarget[i].BlendOp		= ArEnumToDx(state.blend[i].op);
				bsDesc.RenderTarget[i].SrcBlendAlpha	= ArEnumToDx(state.blend[i].srcAlpha);
				bsDesc.RenderTarget[i].DestBlendAlpha	= ArEnumToDx(state.blend[i].dstAlpha);
				bsDesc.RenderTarget[i].BlendOpAlpha	= ArEnumToDx(state.blend[i].opAlpha);
				bsDesc.RenderTarget[i].RenderTargetWriteMask	= state.blend[i].channelMask & D3D11_COLOR_WRITE_ENABLE_ALL;
			}

			HRESULT result = device->CreateBlendState(&bsDesc, &bs);
			if (FAILED(result))
				throw core::NullReferenceException();
			(*bsMap)[bitFilerAssembled] = bs;
		}
		else
			bs = bsPair->second;

		// Set the updated raster state
		m_blendStateCurrentBitfilter[0] = bitFilter[0];
		m_blendStateCurrentBitfilter[1] = bitFilter[1];
		m_blendStateCurrentBitfilter[2] = bitFilter[2];
		m_blendStateCurrentBitfilter[3] = bitFilter[3];

		FLOAT blendFactor [4] = {1, 1, 1, 1};
		ctx->OMSetBlendState(bs, blendFactor, 0xFFFFFFFF);
	}

	return kError_SUCCESS;
}

int gpu::GraphicsContext::setDepthStencilState ( const DepthStencilState& state )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	ID3D11Device*			device = static_cast<gpu::Device*>(m_wrapperDevice)->getNative();
	GPUDepthStencilStateMap* dsMap = (GPUDepthStencilStateMap*)m_depthStateCachedMap;
	uint64_t				bitFilter;

	// Build filter used for fast lookup of existing states
	bitFilter = (state.depthTestEnabled ? 1 : 0)
				| ((state.depthWriteEnabled ? 1 : 0) << 1)
				| ((state.depthFunc & 0x07) << 2)
				| ((state.stencilTestEnabled ? 1 : 0) << 5)
				| (state.stencilReadMask << 6)
				| (state.stencilWriteMask << 14)
				| ((state.stencilOpFrontface.failOp & 0x07) << 22)
				| ((state.stencilOpFrontface.depthFailOp & 0x07) << 25)
				| ((state.stencilOpFrontface.passOp & 0x07) << 28)
				| (uint64_t(state.stencilOpFrontface.func & 0x07) << 31)
				| (uint64_t(state.stencilOpBackface.failOp & 0x07) << 34)
				| (uint64_t(state.stencilOpBackface.depthFailOp & 0x07) << 37)
				| (uint64_t(state.stencilOpBackface.passOp & 0x07) << 40)
				| (uint64_t(state.stencilOpBackface.func & 0x07) << 43);

	// Filter out uneccessary calls
	if (bitFilter != m_rasterStateCurrentBitfilter) // TODO: Check if this filtering actually provides any speed increase, remove if necessary
	{
		// Locate the bitfilter in the cached map
		ID3D11DepthStencilState*	ds;
		auto dsPair = dsMap->find(bitFilter);

		if (dsPair == dsMap->end())
		{
			D3D11_DEPTH_STENCIL_DESC		dsDesc = {};
			dsDesc.DepthEnable		= state.depthTestEnabled ? TRUE : FALSE;
			dsDesc.DepthWriteMask	= state.depthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
			dsDesc.DepthFunc		= ArEnumToDx(state.depthFunc);
			dsDesc.StencilReadMask	= state.stencilReadMask;
			dsDesc.StencilWriteMask	= state.stencilWriteMask;
			dsDesc.FrontFace.StencilFailOp		= ArEnumToDx(state.stencilOpFrontface.failOp);
			dsDesc.FrontFace.StencilDepthFailOp	= ArEnumToDx(state.stencilOpFrontface.depthFailOp);
			dsDesc.FrontFace.StencilPassOp		= ArEnumToDx(state.stencilOpFrontface.passOp);
			dsDesc.FrontFace.StencilFunc		= ArEnumToDx(state.stencilOpFrontface.func);
			dsDesc.BackFace.StencilFailOp		= ArEnumToDx(state.stencilOpFrontface.failOp);
			dsDesc.BackFace.StencilDepthFailOp	= ArEnumToDx(state.stencilOpFrontface.depthFailOp);
			dsDesc.BackFace.StencilPassOp		= ArEnumToDx(state.stencilOpFrontface.passOp);
			dsDesc.BackFace.StencilFunc			= ArEnumToDx(state.stencilOpFrontface.func);

			HRESULT result = device->CreateDepthStencilState(&dsDesc, &ds);
			if (FAILED(result))
				throw core::NullReferenceException();
			(*dsMap)[bitFilter] = ds;
		}
		else
			ds = dsPair->second;

		// Set the updated raster state
		m_depthStateCurrentBitfilter = bitFilter;
		ctx->OMSetDepthStencilState(ds, state.stencilReference);
	}

	return kError_SUCCESS;
}

//===============================================================================================//

int gpu::GraphicsContext::debugGroupPush ( const char* groupName )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	ID3D11DeviceContext2*	ctx2;
	ctx->QueryInterface(&ctx2);
	if (ctx2)
	{
		//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		//std::wstring wide = converter.from_bytes(groupName);

		//ctx2->BeginEventInt(wide.c_str(), 0);

		int groupNameLen = (int)strlen(groupName);
		int wideLen = MultiByteToWideChar(CP_UTF8, 0, groupName, groupNameLen, NULL, NIL);
		
		wchar_t* wide = new wchar_t[wideLen];
		MultiByteToWideChar(CP_UTF8, 0, groupName, groupNameLen, wide, wideLen);

		ctx2->BeginEventInt(wide, 0);

		delete[] wide;
	}
	return kError_SUCCESS;
}

int gpu::GraphicsContext::debugGroupPop ( void )
{
	ID3D11DeviceContext*	ctx = (ID3D11DeviceContext*)m_deferredContext;
	ID3D11DeviceContext2*	ctx2;
	ctx->QueryInterface(&ctx2);
	if (ctx2)
	{
		ctx2->EndEvent();
	}
	return kError_SUCCESS;
}

#endif