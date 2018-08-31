#include "renderer/logic/model/RrModelMasterSubsystem.h"
#include "renderer/logic/model/CAnimatedMeshGroup.h"
#include "core/debug/console.h"

RrModelMasterSubsystem::RrModelMasterSubsystem ( void )
{

}
RrModelMasterSubsystem::~RrModelMasterSubsystem ( void )
{

}

void RrModelMasterSubsystem::Update ( void )
{
	//
}

void RrModelMasterSubsystem::OnAdd(IArResource* resource)
{
	CAnimatedMeshGroup* texture = (CAnimatedMeshGroup*)resource;
	//texture->AddReference();
}

void RrModelMasterSubsystem::OnRemove(IArResource* resource)
{
	CAnimatedMeshGroup* texture = (CAnimatedMeshGroup*)resource;
	//texture->RemoveReference();
}

void RrModelMasterSubsystem::Reload ( void )
{
	/*auto listing = this->GetListing();
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
	}*/
}