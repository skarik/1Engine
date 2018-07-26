#include "renderer/material/RrShaderMasterSubsystem.h"
#include "renderer/material/RrShaderProgram.h"
#include "core/debug/console.h"

RrShaderMasterSubsystem::RrShaderMasterSubsystem ( void )
{

}
RrShaderMasterSubsystem::~RrShaderMasterSubsystem ( void )
{

}

void RrShaderMasterSubsystem::Update ( void )
{
	//
}

void RrShaderMasterSubsystem::OnAdd(IArResource* resource)
{
	RrShaderProgram* program = (RrShaderProgram*)resource;
	//texture->AddReference();
}

void RrShaderMasterSubsystem::OnRemove(IArResource* resource)
{
	RrShaderProgram* program = (RrShaderProgram*)resource;
	//texture->RemoveReference();
}

void RrShaderMasterSubsystem::Reload ( void )
{
	auto listing = this->GetListing();
	for (IArResource* resource : listing)
	{
		RrShaderProgram* program = (RrShaderProgram*)resource;
		if (program != NULL)
		{
			/*if (texture->GetIsProcedural() == false
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget_Cube
				&& texture->ClassType() != core::gfx::tex::kTextureClassRenderTarget_MRT)
			{
				// Ask for a reload.
				texture->Reload();
			}*/
			// todo
		}
	}
}