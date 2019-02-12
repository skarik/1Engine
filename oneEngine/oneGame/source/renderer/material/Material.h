#ifndef RENDERER_MATERIAL_USABILITY_LAYER_H_
#define RENDERER_MATERIAL_USABILITY_LAYER_H_

#include "RrPass.h"
#include "renderer/texture/RrTexture.h"

namespace gpu
{
	class GraphicsContext;
	class Pipeline;
}

namespace renderer
{
	//	Material usability layer.
	// Utility class that consolidates many common shader & pass based functionality.
	class Material
	{
	public:

		RENDER_API explicit Material ( CRenderableObject* thisObject, gpu::GraphicsContext* ctx, int8_t pass, gpu::Pipeline* pipeline )
		{
			m_object = thisObject;
			m_ctx = ctx;
			m_pipeline = pipeline;
			m_pass = &thisObject->m_passes[pass];
		}

		RENDER_API Material& setAll ( CRenderableObject::rrRenderParams* thisParams )
		{
			return setStart().setDepthStencilState().setBlendState().setTextures().setCommonCBuffers(thisParams);
		}

		RENDER_API Material& setStart ( void )
		{
			m_ctx->setPipeline(m_pipeline);
			return *this;
		}

		RENDER_API Material& setDepthStencilState ( void )
		{
			gpu::DepthStencilState ds;
			ds.depthWriteEnabled = m_pass->m_depthWrite;
			ds.depthTestEnabled = true;
			ds.depthFunc = m_pass->m_depthTest;
			ds.stencilTestEnabled = false;

			m_ctx->setDepthStencilState(ds);
			return *this;
		}

		RENDER_API Material& setBlendState ( void )
		{
			gpu::BlendState bs;
			bs.channelMask = 0xFF;
			bs.enable = (m_pass->m_alphaMode == renderer::kAlphaModeTranslucent) ? true : false;
			if (m_pass->m_alphaMode == renderer::kAlphaModeTranslucent)
			{
				bs.dst = gpu::kBlendModeInvSrcAlpha;
				bs.src = gpu::kBlendModeSrcAlpha;
				bs.op = gpu::kBlendOpAdd;

				bs.dstAlpha = gpu::kBlendModeOne;
				bs.srcAlpha = gpu::kBlendModeOne;
				bs.opAlpha = gpu::kBlendOpAdd;
			}

			m_ctx->setBlendState(bs);
			return *this;
		}

		RENDER_API Material& setTextures ( void )
		{
			for (int i = 0; i < kPass_MaxTextureSlots; ++i)
			{
				if (m_pass->m_texturesRaw[i] != NULL) {
					m_ctx->setShaderSamplerAuto(gpu::kShaderStagePs, i, m_pass->m_texturesRaw[i]);
				}
				else if (m_pass->m_textures[i] != NULL) {
					m_ctx->setShaderSamplerAuto(gpu::kShaderStagePs, i, &m_pass->m_textures[i]->GetTexture());
				}
			}
			return *this;
		}

		RENDER_API Material& setCommonCBuffers ( CRenderableObject::rrRenderParams* thisParams )
		{
			m_ctx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_object->m_cbufPerObjectMatrices);
			m_ctx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_object->m_cbufPerObjectSurfaces[thisParams->pass]);
			m_ctx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, thisParams->cbuf_perCamera);
			m_ctx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, thisParams->cbuf_perPass);
			m_ctx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, thisParams->cbuf_perFrame);
			return *this;
		}

	public:
		CRenderableObject*
							m_object;
		gpu::GraphicsContext*
							m_ctx;
		gpu::Pipeline*		m_pipeline;
		RrPass*				m_pass;
	};
};


#endif//RENDERER_MATERIAL_USABILITY_LAYER_H_