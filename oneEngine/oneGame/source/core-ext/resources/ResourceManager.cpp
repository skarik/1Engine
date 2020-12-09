#include "core/utils/string.h"
#include "core/debug/console.h"
#include "core-ext/resources/ResourceManager.h"
#include "core/math/hash.h"

ARSINGLETON_CPP_DEF(core::ArResourceManager);

core::ArResourceManager::ArResourceManager ( void )
{
	;
}
core::ArResourceManager::~ArResourceManager ( void )
{
	;
}

//	Initialize() : Initializes the resource system
void core::ArResourceManager::Initialize ( void )
{
	for (int i_resourceType = 0; i_resourceType < core::kResourceType_MAX; ++i_resourceType)
	{
		m_resourceLoadCounts[i_resourceType] = 0;
		m_resourceSubsystems[i_resourceType] = NULL;
	}
}

//	SetSubsystem() : Adds a subsystem to the resource manager
void core::ArResourceManager::SetSubsystem ( const core::arResourceType resource_type, core::IArResourceSubsystem* subsystem )
{
	if (resource_type != core::kResourceType_MAX)
	{
		subsystem->m_manager = this;
		subsystem->m_subsystemType = resource_type;
		m_resourceSubsystems[resource_type] = subsystem;
	}
	else
	{
		throw core::InvalidArgumentException();
	}
}
//	GetSubsystem() : Adds a subsystem to the resource manager
core::IArResourceSubsystem* core::ArResourceManager::GetSubsystem ( const core::arResourceType resource_type )
{
	if (resource_type != core::kResourceType_MAX)
	{
		return m_resourceSubsystems[resource_type];
	}
	else
	{
		throw core::InvalidArgumentException();
	}
	return NULL;
}

// In non-release mode, checks files for changes
void core::ArResourceManager::FileUpdate ( void )
{
	// TODO: Check all the resources for disk changes:
}

// Step per frame for updating the streaming system
void core::ArResourceManager::Update ( void )
{
	// Loop through all the load counts and check if there is something to load:
	for (int i_resourceType = 0; i_resourceType < core::kResourceType_MAX; ++i_resourceType)
	{
		// Is there something here to load?
		if (m_resourceLoadCounts[i_resourceType] != 0)
		{
			// Loop through the resource list:
			size_t i_max = m_resourceLoadLists[i_resourceType].size();
			for (size_t i = 0; i < i_max; ++i)
			{
				IArResource* resource = m_resourceLoadLists[i_resourceType][i];
				if (resource != NULL)
				{
					// Step the streamed loader:
					bool doneLoading = false;
					if (resource->IsStreamedLoad())
					{
						doneLoading = resource->OnStreamStep(false);
					}
					// Force the streamed loader:
					else
					{
						doneLoading = true;
						while (resource->OnStreamStep(true) == false) {}
					}
					// Remove from load list when done:
					if (doneLoading)
					{
						m_resourceLoadLists[i_resourceType][i] = NULL;
						m_resourceLoadCounts[i_resourceType] -= 1;
					}
				}
			}
		}
		
		// Update the subsystem:
		if (m_resourceSubsystems[i_resourceType] != NULL)
		{
			m_resourceSubsystems[i_resourceType]->Update();
		}
	}

	//
}

IArResource* core::ArResourceManager::Find ( const core::arResourceType resource_type, const char* resource_name )
{
	core::arResourceHash hash = math::hash::fnv1a_32(resource_name);

	// Loop through the resource list, matching the hash...
	size_t i_max = m_resourceLists[resource_type].size();
	for (size_t i = 0; i < i_max; ++i)
	{
		IArResource* resource = m_resourceLists[resource_type][i];
		if (resource != NULL)
		{
			// Check if hash matches
			if (resource->ResourceHash() == hash &&
				// Check if resource name matches
				strcmp(resource->ResourceName(), resource_name) == 0)
			{
				return resource;
			}
		}
	}

	return NULL;
}

//	Contains() : Checks if resource is already in the system
bool core::ArResourceManager::Contains ( IArResource* resource )
{
	core::arResourceType resource_type = resource->ResourceType();

	// This is simple. We find the resource...
	size_t i_max = m_resourceLists[resource_type].size();
	for (size_t i = 0; i < i_max; ++i)
	{
		if (m_resourceLists[resource_type][i] == resource)
		{
			return true;
		}
	}
	return false;
}

void core::ArResourceManager::Add ( IArResource* resource )
{
	core::arResourceType resource_type = resource->ResourceType();
	resource->InitResourceHash();

	// Add to the resource listing.
	m_resourceLists[resource_type].push_back(resource);

	// Update streamed check 
	bool streamed = resource->IsStreamedLoad();
	// Apply current settings
	if (resource_type == core::kResourceTypeRrTexture
		|| resource_type == core::kResourceTypeRawImage)
	{
		if (m_settings.streamTextures == false) {
			streamed = false;
		}
	}

	// Is it streamed?
	if (streamed)
	{
		m_resourceLoadLists[resource_type].push_back(resource);
		m_resourceLoadCounts[resource_type] += 1;
		// Perform first stream step immediately:
		resource->OnStreamStep(false);
	}
	else
	{
		// Not streamed? Loop until we're done loading.
		while (resource->OnStreamStep(true) == false) {
			// Loading.
		}
	}

	// Update the subsystem:
	if (m_resourceSubsystems[resource_type] != NULL)
	{
		m_resourceSubsystems[resource_type]->OnAdd(resource);
	}
}

void core::ArResourceManager::Remove ( IArResource* resource )
{
	core::arResourceType resource_type = resource->ResourceType();

	// Update the subsystem:
	if (m_resourceSubsystems[resource_type] != NULL)
	{
		m_resourceSubsystems[resource_type]->OnRemove(resource);
	}

	// This is simple. We find the resource...
	size_t i_max = m_resourceLists[resource_type].size();
	for (size_t i = 0; i < i_max; ++i)
	{
		if (m_resourceLists[resource_type][i] == resource)
		{
			// And null it out:
			m_resourceLists[resource_type][i] = NULL;
		}
	}
}