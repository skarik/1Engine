#include "IPrVolume.h"
#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"

IPrVolume::IPrVolume ( PrWorld * world )
	: m_world(world)
{
	// Set default world
	if (m_world == NULL)
	{
		m_world = PrPhysics::Active()->CurrentWorld();
	}
	
	// Add self to the world's listing
	m_world->AddVolume( this );
}

IPrVolume::~IPrVolume ( void )
{
	m_world->RemoveVolume( this );
	m_world = NULL;
}