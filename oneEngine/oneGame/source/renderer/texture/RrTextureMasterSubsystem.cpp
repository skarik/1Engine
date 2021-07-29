#include "renderer/texture/RrTextureMasterSubsystem.h"
#include "renderer/texture/RrTexture.h"
#include "core/debug/console.h"

#include "gpuw/GraphicsContext.h"

RrTextureMasterSubsystem::RrTextureMasterSubsystem ( gpu::GraphicsContext* context )
	: graphics_context(context)
{}
RrTextureMasterSubsystem::~RrTextureMasterSubsystem ( void )
{}

void RrTextureMasterSubsystem::Update ( void )
{
	// Submit commands
	graphics_context->submit();
	// Clear out all that's been submitted
	graphics_context->reset();
}

// Called between stream steps of a non-streamed resource. Useful for if syncronization with an external system needs to happen.
void RrTextureMasterSubsystem::OnBetweenStreamStep ( IArResource* resource )
{
	// Submit commands so we don't stall (since we're now working on a deferred context)
	//graphics_context->submit();
	// Clear out all that's been submitted
	//graphics_context->reset();
}

void RrTextureMasterSubsystem::OnAdd(IArResource* resource)
{
	RrTexture* texture = (RrTexture*)resource;
	//texture->AddReference();
}

void RrTextureMasterSubsystem::OnRemove(IArResource* resource)
{
	RrTexture* texture = (RrTexture*)resource;
	//texture->RemoveReference();
}

void RrTextureMasterSubsystem::Reload ( void )
{
	auto listing = this->GetListing();
	for (IArResource* resource : listing)
	{
		RrTexture* texture = (RrTexture*)resource;
		if (texture != NULL)
		{
			if (texture->GetIsProcedural() == false
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget_Cube
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget_MRT)
			{
				// Ask for a reload.
				texture->Reload();
			}
		}
	}
}