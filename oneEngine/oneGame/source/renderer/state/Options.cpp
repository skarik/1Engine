#include "renderer/state/Options.h"

#include "renderer/state/CRenderState.h"
#include "renderer/resource/CResourceManager.h"
#include "renderer/material/RrPassDeferred.h"

void renderer::Options::TextureStreaming ( const bool enabled )
{
	CRenderState::Active->mResourceManager->settings.streamTextures = enabled;
}

void renderer::Options::DeferredShaders ( const rrDeferredShaderSettings& shaderInfo )
{
	renderer::gDefferedShaderSettings = shaderInfo;
}