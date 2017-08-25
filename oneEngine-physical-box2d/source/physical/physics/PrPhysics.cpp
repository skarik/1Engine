#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"

ARSINGLETON_CPP_DEF(PrPhysics);

PrPhysics::PrPhysics ( void )
{
	m_activeWorldIndex = -1;
	m_collisionMasks = NULL;

	m_simulationTime = 0.0;
	m_simulationTarget = 0.0;
}
PrPhysics::~PrPhysics ( void )
{
	// Delete all existing worlds:
	m_worlds_lock.lock();
	for (auto world : m_worlds) {
		delete world;
	}
	m_worlds.clear();
	m_worlds_lock.unlock();

	// Free collision mask data:
	if (m_collisionMasks != NULL) {
		delete[] m_collisionMasks;
		m_collisionMasks = NULL;
	}

	// Free engine:
	Free_Engine();
}

//	AddWorld (world) : adds a world to the update list.
void PrPhysics::AddWorld ( PrWorld* world_to_add_update )
{
	std::unique_lock<std::mutex> lock (m_worlds_lock); // Scope locked the list during access

	auto findResult = std::find(m_worlds.begin(), m_worlds.end(), world_to_add_update);
	if ( findResult == m_worlds.end() )
	{
		m_worlds.push_back(world_to_add_update);
	}
	else
	{
		throw core::InvalidCallException();
	}

	// If invalid world is selected, select the newly added world:
	if (m_activeWorldIndex < 0 || m_activeWorldIndex >= (int)m_worlds.size())
	{
		m_activeWorldIndex = (int)(m_worlds.size() - 1);
	}
}
//	RemoveWorld (world) : removes world from the update list
void PrPhysics::RemoveWorld	( PrWorld* world_to_remove_update )
{
	std::unique_lock<std::mutex> lock (m_worlds_lock); // Scope locked the list during access

	auto findResult = std::find(m_worlds.begin(), m_worlds.end(), world_to_remove_update);
	if ( findResult == m_worlds.end() )
	{
		throw core::NoWorldException();
	}
	else
	{
		m_worlds.erase(findResult);
	}
}

//	CurrentWorld() : returns the currently active world
// Returns NULL if no world is active.
PrWorld* PrPhysics::CurrentWorld ( void )
{
	std::unique_lock<std::mutex> lock (m_worlds_lock); // Scope locked the list during access

	if (m_activeWorldIndex >= 0 && m_activeWorldIndex < (int)m_worlds.size())
	{
		return m_worlds[m_activeWorldIndex];
	}
	return NULL;
}