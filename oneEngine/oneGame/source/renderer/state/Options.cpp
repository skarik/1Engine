
#include "renderer/state/Options.h"

#include "renderer/state/CRenderState.h"
#include "renderer/resource/CResourceManager.h"

using namespace renderer;

void Options::TextureStreaming ( const bool enabled )
{
	CRenderState::Active->mResourceManager->settings.streamTextures = enabled;
}