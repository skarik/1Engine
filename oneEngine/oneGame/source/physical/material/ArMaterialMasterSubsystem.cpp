#include "physical/material/ArMaterialMasterSubsystem.h"
#include "physical/material/ArMaterial.h"
#include "core/debug/console.h"

ArMaterialMasterSubsystem::ArMaterialMasterSubsystem ( void )
{

}
ArMaterialMasterSubsystem::~ArMaterialMasterSubsystem ( void )
{

}

void ArMaterialMasterSubsystem::Update ( void )
{
	//
}

void ArMaterialMasterSubsystem::OnAdd(IArResource* resource)
{
	ArMaterialContainer* container = (ArMaterialContainer*)resource;
	//
}

void ArMaterialMasterSubsystem::OnRemove(IArResource* resource)
{
	ArMaterialContainer* container = (ArMaterialContainer*)resource;
	//
}

void ArMaterialMasterSubsystem::Reload ( void )
{
	//
}