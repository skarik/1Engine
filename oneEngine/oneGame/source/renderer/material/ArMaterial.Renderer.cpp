#include "ArMaterial.Renderer.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/material/RrPass.h"

//===============================================================================================//
// ArMaterial access/edit
//===============================================================================================//

RrTexture* ArMaterialGetTexture ( ArMaterial::RenderInfo::TextureEntry& texture_entry )
{
	if (texture_entry.cached_texture == nullptr)
	{
		if (texture_entry.resource_name.compare("default"))
		{
			texture_entry.cached_texture = RrTexture::Load(renderer::kTextureWhite);
		}
		else if (texture_entry.resource_name.compare("default_normal"))
		{
			texture_entry.cached_texture = RrTexture::Load(renderer::kTextureNormalN0);
		}
		else if (texture_entry.resource_name.compare("default_surface"))
		{
			texture_entry.cached_texture = RrTexture::Load(renderer::kTextureSurfaceM0);
		}
		else if (texture_entry.resource_name.compare("default_overlay"))
		{
			texture_entry.cached_texture = RrTexture::Load(renderer::kTextureGrayA0);
		}
		else if (texture_entry.resource_name.compare("none"))
		{
			texture_entry.cached_texture = RrTexture::Load(renderer::kTextureWhite);
		}
		else
		{
			texture_entry.cached_texture = RrTexture::Load( texture_entry.resource_name );
		}
	}
	return texture_entry.cached_texture;
}

//===============================================================================================//
// RrPass setup, dealing with ArMaterial
//===============================================================================================//

void RrPass::utilSetupFromMaterial ( ArMaterial* material )
{
	utilSetupAsDefault();

	switch (material->render_info.render_mode)
	{
	case ArRenderMode::kLitOpaque:
	case ArRenderMode::kLitFoliage:
		m_type = kPassTypeDeferred;
		break;

	case ArRenderMode::kFastDecal:
		ARCORE_ERROR("Unimplemented.");
		break;
	}

	// Set up various render options
	m_alphaMode = material->render_info.alpha_test > 0.0F ? renderer::kAlphaModeAlphatest : renderer::kAlphaModeNone;
	switch (material->render_info.cull)
	{
	case ArFacingCullMode::kNone:
		m_cullMode = gpu::kCullModeNone;
		break;
	case ArFacingCullMode::kBack:
		m_cullMode = gpu::kCullModeBack;
		break;
	case ArFacingCullMode::kFront:
		m_cullMode = gpu::kCullModeFront;
		break;
	}

	// Set up all surface
	m_surface.diffuseColor = material->render_info.diffuse_color;
	m_surface.alphaCutoff = material->render_info.alpha_test;
	m_surface.textureScale = material->render_info.repeat_factor;
	m_surface.baseSmoothness = material->render_info.smoothness_bias;
	m_surface.scaledSmoothness = material->render_info.smoothness_scale;
	m_surface.baseMetallicness = material->render_info.metallicness_bias;
	m_surface.scaledMetallicness = material->render_info.metallicness_scale;
	switch (material->render_info.render_mode)
	{
	case ArRenderMode::kLitOpaque:
		m_surface.shadingModel = kShadingModelNormal;
		break;
	case ArRenderMode::kLitFoliage:
		m_surface.shadingModel = kShadingModelThinFoliage;
		break;

	case ArRenderMode::kFastDecal:
		ARCORE_ERROR("Unimplemented.");
		break;
	}

	// Set up textures
	setTexture( TEX_DIFFUSE, ArMaterialGetTexture(material->render_info.texture_diffuse) );
	setTexture( TEX_NORMALS, ArMaterialGetTexture(material->render_info.texture_normals) );
	setTexture( TEX_SURFACE, ArMaterialGetTexture(material->render_info.texture_surface) );
	setTexture( TEX_OVERLAY, ArMaterialGetTexture(material->render_info.texture_overlay) );

	// Set up texture filtering
	gpu::SamplerCreationDescription pointFilter;
	pointFilter.minFilter = material->render_info.sampling;
	pointFilter.magFilter = material->render_info.sampling;
	setSampler(rrTextureSlot::TEX_DIFFUSE, &pointFilter);
	setSampler(rrTextureSlot::TEX_NORMALS, &pointFilter);
	setSampler(rrTextureSlot::TEX_SURFACE, &pointFilter);
	setSampler(rrTextureSlot::TEX_OVERLAY, &pointFilter);

	// Set up the vertex specification based on what's expected for the given shaders.
	// Since the shader defines the expected input, we can be sure this definition here will not cause issues.
	setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{(material->render_info.shader_vv + ".spv"), (material->render_info.shader_p + ".spv")}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
		renderer::shader::Location::kUV0,
		renderer::shader::Location::kColor,
		renderer::shader::Location::kNormal,
		renderer::shader::Location::kTangent,
		renderer::shader::Location::kBinormal};
	setVertexSpecificationByCommonList(t_vspec, sizeof(t_vspec) / sizeof(renderer::shader::Location));

	// Materials are likely going to be used with a triangle list (TODO, we want to optimize to strips)
	// This can be overriden by the Pass creator.
	m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
}