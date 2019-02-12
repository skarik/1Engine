#include "core-ext/resources/IArResourceSubsystem.h"
#include "core-ext/resources/ResourceManager.h"

std::vector<IArResource*>&
core::IArResourceSubsystem::GetListing ( void )
{
	return m_manager->m_resourceLists[m_subsystemType];
}