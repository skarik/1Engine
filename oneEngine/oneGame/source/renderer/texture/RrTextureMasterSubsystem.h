#ifndef RENDERER_TEXTURE_MASTER_SUBSYSTEM_H_
#define RENDERER_TEXTURE_MASTER_SUBSYSTEM_H_

#include "core-ext/resources/IArResourceSubsystem.h"

class RrTexture;
class RrTextureMasterSubsystem : public core::IArResourceSubsystem
{
public:
	explicit			RrTextureMasterSubsystem ( void );
						~RrTextureMasterSubsystem ( void );

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

#endif//RENDERER_TEXTURE_MASTER_SUBSYSTEM_H_