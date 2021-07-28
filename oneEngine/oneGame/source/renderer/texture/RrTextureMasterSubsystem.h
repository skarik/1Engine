#ifndef RENDERER_TEXTURE_MASTER_SUBSYSTEM_H_
#define RENDERER_TEXTURE_MASTER_SUBSYSTEM_H_

#include "core-ext/resources/IArResourceSubsystem.h"

namespace gpu
{
	class GraphicsContext;
}

class RrTexture;
class RrTextureMasterSubsystem : public core::IArResourceSubsystem
{
public:
	explicit				RrTextureMasterSubsystem ( gpu::GraphicsContext* context );
							~RrTextureMasterSubsystem ( void );

protected:
	// Step per frame for updating the streaming system
	void					Update ( void ) override;

	// Called between stream steps of a non-streamed resource. Useful for if syncronization with an external system needs to happen.
	void					OnBetweenStreamStep ( IArResource* resource ) override;

	//	OnAdd() : Called when adding resource to be tracked by resource system.
	void					OnAdd ( IArResource* resource ) override;
	//	OnRemove() : Called when removing resource from tracking.
	void					OnRemove ( IArResource* resource ) override;

	//	IsSubsystemAutoUpdated() : Is this subsystem automatically updated by the resource system?
	bool					IsSubsystemAutoUpdated ( void ) override
		{ return false; } // Updated by the renderer
	//	IsResourceTypeAutoUpdated() : Are the resources this subsystem lords over automatically updated by the resource system?
	bool					IsResourceTypeAutoUpdated ( void ) override
		{ return false; } // Updated by the renderer

public:
	//	Reload() : Called when clients want to reload subsytem's values.
	void					Reload ( void ) override;

public:
	gpu::GraphicsContext*	GetGraphicsContext ( void )
		{ return graphics_context; }
	void					SetGraphicsContext ( gpu::GraphicsContext* context )
		{ graphics_context = context; }

protected:
	gpu::GraphicsContext*
						graphics_context;
};

#endif//RENDERER_TEXTURE_MASTER_SUBSYSTEM_H_