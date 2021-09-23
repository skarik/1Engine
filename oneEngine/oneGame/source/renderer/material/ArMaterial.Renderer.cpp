#include "ArMaterial.Renderer.h"

#include "renderer/texture/RrTexture.h"

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