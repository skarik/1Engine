#ifndef CORE_RESOURCE_INTERFACE_RESOURCE_SUBSYSTEM_MANAGER_H_
#define CORE_RESOURCE_INTERFACE_RESOURCE_SUBSYSTEM_MANAGER_H_

#include "core/types.h"
#include "core-ext/resources/IArResource.h"
#include <vector>

namespace core
{
	class ArResourceManager;

	// Interface for a "Master" subsystem inside the resource system.
	class IArResourceSubsystem
	{
	public:
		CORE_API virtual		~IArResourceSubsystem ( void ) {}

	protected:
		friend ArResourceManager;
		ArResourceManager*		m_manager;
		core::arResourceType	m_subsystemType;

		//CORE_API const std::vector<IArResource*>&
		//						GetListing ( void ) const;
		CORE_API std::vector<IArResource*>&
								GetListing ( void );

	protected:
		// Step per frame for updating the streaming system
		CORE_API virtual void	Update ( void ) = 0;

		// Called between stream steps of a non-streamed resource. Useful for if syncronization with an external system needs to happen.
		CORE_API virtual void	OnBetweenStreamStep ( IArResource* resource )
			{}

		//	OnAdd() : Called when adding resource to be tracked by resource system.
		CORE_API virtual void	OnAdd ( IArResource* resource )
			{}
		//	OnRemove() : Called when removing resource from tracking.
		CORE_API virtual void	OnRemove ( IArResource* resource )
			{}

		//	IsSubsystemAutoUpdated() : Is this subsystem automatically updated by the resource system?
		CORE_API virtual bool	IsSubsystemAutoUpdated ( void ) 
			{ return true; }
		//	IsResourceTypeAutoUpdated() : Are the resources this subsystem lords over automatically updated by the resource system?
		CORE_API virtual bool	IsResourceTypeAutoUpdated ( void )
			{ return true; }
	public:
		//	Reload() : Called when clients want to reload subsytem's values.
		CORE_API virtual void	Reload ( void )
			{}
	};
}

#endif//CORE_RESOURCE_INTERFACE_RESOURCE_SUBSYSTEM_MANAGER_H_