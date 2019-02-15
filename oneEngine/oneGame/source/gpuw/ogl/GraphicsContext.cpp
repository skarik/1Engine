#include "gpuw/gpuw_common.h"
#ifdef GPU_API_OPENGL

#include "./GraphicsContext.h"

#include "./ogl/GLCommon.h"
#include "renderer/types/types.h"

#include "core/types/types.h"
#include "core/exceptions.h"

static gpuEnum GpuEnumToGL ( const gpu::BlendMode bm )
{
	using namespace gpu;
	switch (bm)
	{
	case kBlendModeZero:			return GL_ZERO;
	case kBlendModeOne:				return GL_ONE;
	case kBlendModeSrcColor:		return GL_SRC_COLOR;
	case kBlendModeInvSrcColor:		return GL_ONE_MINUS_SRC_COLOR;
	case kBlendModeSrcAlpha:		return GL_SRC_ALPHA;
	case kBlendModeInvSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
	case kBlendModeDstColor:		return GL_DST_COLOR;
	case kBlendModeInvDstColor:		return GL_ONE_MINUS_DST_COLOR;
	case kBlendModeDstAlpha:		return GL_DST_ALPHA;
	case kBlendModeInvDstAlpha:		return GL_ONE_MINUS_DST_ALPHA;
	case kBlendModeSrcAlphaSat:		return GL_SRC_ALPHA_SATURATE;
	case kBlendModeBlendFactor:		return GL_CONSTANT_COLOR;
	case kBlendModeInvBlendFactor:	return GL_ONE_MINUS_CONSTANT_COLOR;
	case kBlendModeSrc1Color:		return GL_SRC1_COLOR;
	case kBlendModeInvSrc1Color:	return GL_ONE_MINUS_SRC1_COLOR;
	case kBlendModeSrc1Alpha:		return GL_SRC1_ALPHA;
	case kBlendModeInvSrc1Alpha:	return GL_ONE_MINUS_SRC1_ALPHA;
	}
	throw core::InvalidArgumentException();
	return GL_INVALID_ENUM;
}

static gpuEnum GpuEnumToGL ( const gpu::BlendOp bm )
{
	using namespace gpu;
	switch (bm)
	{
	case kBlendOpAdd:			return GL_FUNC_ADD;
	case kBlendOpSubtract:		return GL_FUNC_SUBTRACT;
	case kBlendOpRevSubtract:	return GL_FUNC_REVERSE_SUBTRACT;
	case kBlendOpMin:			return GL_MIN;
	case kBlendOpMax:			return GL_MAX;
	}
	throw core::InvalidArgumentException();
	return GL_INVALID_ENUM;
}

static gpuEnum GpuEnumToGL ( const gpu::CompareOp op )
{
	using namespace gpu;
	switch (op)
	{
	case kCompareOpNever:			return GL_NEVER;
	case kCompareOpLess:			return GL_LESS;
	case kCompareOpEqual:			return GL_EQUAL;
	case kCompareOpLessEqual:		return GL_LEQUAL;
	case kCompareOpGreater:			return GL_GREATER;
	case kCompareOpNotEqual:		return GL_NOTEQUAL;
	case kCompareOpGreaterEqual:	return GL_GEQUAL;
	case kCompareOpAlways:			return GL_ALWAYS;
	}
	throw core::InvalidArgumentException();
	return GL_INVALID_ENUM;
}

static gpuEnum GpuEnumToGL ( const gpu::StencilOp op )
{
	using namespace gpu;
	switch (op)
	{
	case kStencilOpKeep:				return GL_KEEP;
	case kStencilOpZero:				return GL_ZERO;
	case kStencilOpReplace:				return GL_REPLACE;
	case kStencilOpIncrementSaturate:	return GL_INCR;
	case kStencilOpDecrementSaturate:	return GL_DECR;
	case kStencilOpInvert:				return GL_INVERT;
	case kStencilOpIncrement:			return GL_INCR_WRAP;
	case kStencilOpDecrement:			return GL_DECR_WRAP;
	}
	throw core::InvalidArgumentException();
	return GL_INVALID_ENUM;
}

//	DeviceSetFillMode( device, fillMode ) : Set device's fill mode.
// Controls how to fill polygons for given device. (glPolygonMode in OpenGL)
// NULL device sets for current active device.
int gpu::GraphicsContext::setFillMode( const FillMode fillMode )
{
	if ( fillMode == kFillModeWireframe )
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	else if ( fillMode == kFillModeSolid )
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	return 0;
}

int gpu::GraphicsContext::setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
{
	glViewport( left, top, right-left, bottom-top );
	return 0;
}

int gpu::GraphicsContext::setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom )
{
	glScissor( left, top, right-left, bottom-top );
	return 0;
}

int gpu::GraphicsContext::setRasterizerState ( const RasterizerState& state )
{
	if (state.scissorEnabled != m_rasterState.scissorEnabled)
	{
		if (state.scissorEnabled)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
		m_rasterState.scissorEnabled = state.scissorEnabled;
	}

	if (state.fillmode != m_rasterState.fillmode)
	{
		if (state.fillmode == kFillModeWireframe)
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		else if (state.fillmode == kFillModeSolid)
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		m_rasterState.fillmode = state.fillmode;
	}

	if (state.cullmode != m_rasterState.cullmode)
	{
		if (state.cullmode == kCullModeFront) {
			glEnable( GL_CULL_FACE );
			glCullFace( GL_BACK );
		}
		else if (state.cullmode == kCullModeBack) {
			glEnable( GL_CULL_FACE );
			glCullFace( GL_FRONT );
		}
		else if (state.cullmode == kCullModeNone) {
			glDisable( GL_CULL_FACE );
		}
		m_rasterState.cullmode = state.cullmode;
	}

	if (state.frontface != m_rasterState.frontface)
	{
		if (state.frontface == kFrontFaceCounterClockwise) {
			glFrontFace(GL_CCW);
		}
		else if (state.frontface == kFrontFaceClockwise) {
			glFrontFace(GL_CW);
		}
		m_rasterState.frontface = state.frontface;
	}

	return 0;
}

int gpu::GraphicsContext::setBlendState ( const BlendState& state )
{
	bool changed = false;
	// change the blend state
	if (state.enable != m_blendCollectState.blend[0].enable)
	{
		if (state.enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		changed = true;
	}
	// change the blend function
	if (state.src != m_blendCollectState.blend[0].src
		|| state.dst != m_blendCollectState.blend[0].dst
		|| state.srcAlpha != m_blendCollectState.blend[0].srcAlpha
		|| state.dstAlpha != m_blendCollectState.blend[0].dstAlpha
		)
	{
		glBlendFuncSeparate(GpuEnumToGL(state.src), GpuEnumToGL(state.dst),
			                GpuEnumToGL(state.srcAlpha), GpuEnumToGL(state.dstAlpha));
		changed = true;
	}
	// change blend equation
	if (state.op != m_blendCollectState.blend[0].op
		|| state.opAlpha != m_blendCollectState.blend[0].opAlpha
		)
	{
		glBlendEquationSeparate(GpuEnumToGL(state.op), GpuEnumToGL(state.opAlpha));
		changed = true;
	}
	if (changed)
	{	// update prev state
		m_blendCollectState.blend[0] = state;
	}
	return 0;
}

int gpu::GraphicsContext::setBlendCollectiveState ( const BlendCollectiveState& state )
{
	bool changed = false;
	for (int i = 0; i < 16; ++i)
	{
		// change the blend state
		if (state.blend[i].enable != m_blendCollectState.blend[i].enable)
		{
			if (state.blend[i].enable)
				glEnablei(GL_BLEND, i);
			else
				glDisablei(GL_BLEND, i);
			changed = true;
		}
		// change the blend function
		if (state.blend[i].src != m_blendCollectState.blend[i].src
			|| state.blend[i].dst != m_blendCollectState.blend[i].dst
			|| state.blend[i].srcAlpha != m_blendCollectState.blend[i].srcAlpha
			|| state.blend[i].dstAlpha != m_blendCollectState.blend[i].dstAlpha
			)
		{
			glBlendFuncSeparatei(i, GpuEnumToGL(state.blend[i].src), GpuEnumToGL(state.blend[i].dst),
				                    GpuEnumToGL(state.blend[i].srcAlpha), GpuEnumToGL(state.blend[i].dstAlpha));
			changed = true;
		}
		// change blend equation
		if (state.blend[i].op != m_blendCollectState.blend[i].op
			|| state.blend[i].opAlpha != m_blendCollectState.blend[i].opAlpha
			)
		{
			glBlendEquationSeparatei(i, GpuEnumToGL(state.blend[i].op), GpuEnumToGL(state.blend[i].opAlpha));
			changed = true;
		}
	}
	if (changed)
	{	// update prev state
		m_blendCollectState = state;
	}
	return 0;
}

int gpu::GraphicsContext::setDepthStencilState ( const DepthStencilState& state )
{
	// Update depth settings
	if (state.depthTestEnabled != m_depthStencilState.depthTestEnabled)
	{
		if (state.depthTestEnabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		m_depthStencilState.depthTestEnabled = state.depthTestEnabled;
	}
	if (state.depthTestEnabled && (state.depthFunc != m_depthStencilState.depthFunc))
	{
		glDepthFunc(GpuEnumToGL(state.depthFunc));
		m_depthStencilState.depthFunc = state.depthFunc;
	}
	if (state.depthWriteEnabled != m_depthStencilState.depthWriteEnabled)
	{
		if (state.depthWriteEnabled)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
		m_depthStencilState.depthWriteEnabled = state.depthWriteEnabled;
	}

	if (state.stencilTestEnabled != m_depthStencilState.stencilTestEnabled)
	{
		if (state.stencilTestEnabled)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
		m_depthStencilState.stencilTestEnabled = state.stencilTestEnabled;
	}
	if (state.stencilTestEnabled)
	{
		if (state.stencilReadMask != m_depthStencilState.stencilReadMask
			|| state.stencilOpFrontface.func != m_depthStencilState.stencilOpFrontface.func)
		{
			glStencilFuncSeparate(GL_FRONT, GpuEnumToGL(state.stencilOpFrontface.func), 0x01, state.stencilReadMask);
			m_depthStencilState.stencilOpFrontface.func = state.stencilOpFrontface.func;
		}
		if (state.stencilReadMask != m_depthStencilState.stencilReadMask
			|| state.stencilOpBackface.func != m_depthStencilState.stencilOpBackface.func)
		{
			glStencilFuncSeparate(GL_BACK, GpuEnumToGL(state.stencilOpBackface.func), 0x01, state.stencilReadMask);
			m_depthStencilState.stencilOpBackface.func = state.stencilOpBackface.func;
		}
		m_depthStencilState.stencilReadMask = state.stencilReadMask;

		if (state.stencilOpFrontface.passOp != m_depthStencilState.stencilOpFrontface.passOp
			|| state.stencilOpFrontface.failOp != m_depthStencilState.stencilOpFrontface.failOp
			|| state.stencilOpFrontface.depthFailOp != m_depthStencilState.stencilOpFrontface.depthFailOp)
		{
			glStencilOpSeparate(GL_FRONT, GpuEnumToGL(state.stencilOpFrontface.failOp),
				                          GpuEnumToGL(state.stencilOpFrontface.depthFailOp),
				                          GpuEnumToGL(state.stencilOpFrontface.passOp));

			m_depthStencilState.stencilOpFrontface.passOp = state.stencilOpFrontface.passOp;
			m_depthStencilState.stencilOpFrontface.failOp = state.stencilOpFrontface.failOp;
			m_depthStencilState.stencilOpFrontface.depthFailOp = state.stencilOpFrontface.depthFailOp;
		}
		if (state.stencilOpBackface.passOp != m_depthStencilState.stencilOpBackface.passOp
			|| state.stencilOpBackface.failOp != m_depthStencilState.stencilOpBackface.failOp
			|| state.stencilOpBackface.depthFailOp != m_depthStencilState.stencilOpBackface.depthFailOp)
		{
			glStencilOpSeparate(GL_BACK, GpuEnumToGL(state.stencilOpBackface.failOp),
				                         GpuEnumToGL(state.stencilOpBackface.depthFailOp),
				                         GpuEnumToGL(state.stencilOpBackface.passOp));

			m_depthStencilState.stencilOpBackface.passOp = state.stencilOpBackface.passOp;
			m_depthStencilState.stencilOpBackface.failOp = state.stencilOpBackface.failOp;
			m_depthStencilState.stencilOpBackface.depthFailOp = state.stencilOpBackface.depthFailOp;
		}

		if (state.stencilWriteMask != m_depthStencilState.stencilWriteMask)
		{
			glStencilMaskSeparate(GL_FRONT, state.stencilWriteMask);
			glStencilMaskSeparate(GL_BACK,  state.stencilWriteMask);
			m_depthStencilState.stencilWriteMask = state.stencilWriteMask;
		}
	}

	return 0;
}

#endif