#include "renderer/texture/RrTextureMasterSubsystem.h"
#include "renderer/texture/RrTexture.h"
#include "core/debug/console.h"

RrTextureMasterSubsystem::RrTextureMasterSubsystem ( void )
{

}
RrTextureMasterSubsystem::~RrTextureMasterSubsystem ( void )
{

}

void RrTextureMasterSubsystem::Update ( void )
{
	//
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