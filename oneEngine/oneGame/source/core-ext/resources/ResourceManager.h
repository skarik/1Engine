#ifndef CORE_RESOURCE_MANAGER_H_
#define CORE_RESOURCE_MANAGER_H_

#include "core/types/types.h"
#include "core/containers/arsingleton.h"
#include "core-ext/resources/IArResource.h"
#include "core-ext/resources/IArResourceSubsystem.h"

#include <thread>
#include <atomic>
#include <shared_mutex>
#include <vector>

namespace core
{
	struct arResourceManagerSettings
	{
		// Are textures streamed on default?
		bool	streamTextures;

		arResourceManagerSettings()
			: streamTextures(true)
		{}
	};

	class ArResourceManager
	{
		ARSINGLETON_H_STORAGE(ArResourceManager, CORE_API);
		ARSINGLETON_H_ACCESS (ArResourceManager);
	private:
		CORE_API explicit		ArResourceManager ( void );
		CORE_API				~ArResourceManager( void );

	public:
		//	Initialize() : Initializes the resource system
		CORE_API void			Initialize	( void );

		//	SetSubsystem() : Adds a subsystem to the resource manager
		CORE_API void			SetSubsystem ( const core::arResourceType resource_type, core::IArResourceSubsystem* subsystem );
		//	GetSubsystem() : Adds a subsystem to the resource manager
		CORE_API core::IArResourceSubsystem*
								GetSubsystem ( const core::arResourceType resource_type );

		//	Find() : Locates resource with matching name.
		CORE_API IArResource*	Find ( const core::arResourceType resource_type, const char* resource_name );
		//	Contains() : Checks if resource is already in the system
		CORE_API bool			Contains ( IArResource* resource );

		//	Add() : Adds resource to be tracked by resource system.
		CORE_API void			Add ( IArResource* resource );
		//	Remove() : Removes resource from tracking.
		CORE_API void			Remove ( IArResource* resource );

		//=========================================//
		// Frame Step
		//=========================================//

		// In non-release mode, checks files for changes
		CORE_API void			FileUpdate ( void );

		// Step per frame for updating the streaming system
		CORE_API void			Update ( void );

	public:
		core::arResourceManagerSettings
							m_settings;

	private:
		// Allows accessors in subsystems to work.
		friend core::IArResourceSubsystem;

		//=========================================//
		// Resource lists
		//=========================================//

		std::vector<IArResource*>
							m_resourceLoadLists [core::kResourceType_MAX];
		int					m_resourceLoadCounts [core::kResourceType_MAX];
		std::vector<IArResource*>
							m_resourceLists [core::kResourceType_MAX];
		std::shared_mutex	m_resourceLocks [core::kResourceType_MAX];
		core::IArResourceSubsystem*
							m_resourceSubsystems [core::kResourceType_MAX];

		//=========================================//
		// Internal state
		//=========================================//

		std::thread			m_streamThread;
		std::atomic<bool>	m_streamFlag;
	};
}

#endif//CORE_RESOURCE_MANAGER_H_