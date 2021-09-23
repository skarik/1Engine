#ifndef CORE_PHYSICAL_MATERIAL_MASTER_SUBSYSTEM_H_
#define CORE_PHYSICAL_MATERIAL_MASTER_SUBSYSTEM_H_

#include "core-ext/resources/IArResourceSubsystem.h"

class ArMaterialMasterSubsystem : public core::IArResourceSubsystem
{
public:
	explicit			ArMaterialMasterSubsystem ( void );
						~ArMaterialMasterSubsystem ( void );

protected:
	// Step per frame for updating the streaming system
	void				Update ( void ) override;

	//	OnAdd() : Called when adding resource to be tracked by resource system.
	void				OnAdd ( IArResource* resource ) override;
	//	OnRemove() : Called when removing resource from tracking.
	void				OnRemove ( IArResource* resource ) override;

public:
	//	Reload() : Called when clients want to reload subsytem's values.
	void				Reload ( void ) override;

};

#endif//CORE_PHYSICAL_MATERIAL_MASTER_SUBSYSTEM_H_