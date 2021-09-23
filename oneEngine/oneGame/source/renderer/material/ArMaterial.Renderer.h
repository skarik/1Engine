#ifndef RENDERER_MATERIAL_CORE_MATERIAL_EXTENSION_H_
#define RENDERER_MATERIAL_CORE_MATERIAL_EXTENSION_H_

#include "core/types.h"
#include "physical/material/ArMaterial.h"

class RrTexture;

RENDER_API RrTexture*		ArMaterialGetTexture ( ArMaterial::RenderInfo::TextureEntry& texture_entry );

#endif//RENDERER_MATERIAL_CORE_MATERIAL_EXTENSION_H_