
#include "Area2DBase.h"

using namespace Engine2D;

std::vector<Area2DBase*>	Area2DBase::m_areas;

Area2DBase::Area2DBase ( void )
	: CGameBehavior()
{
	layer |= Layers::Points;
	// Add this area to the list
	m_areas.push_back( this );
}
Area2DBase::~Area2DBase ( void )
{
	// Remove this area from the list
	m_areas.erase( std::find( m_areas.begin(), m_areas.end(), this ) );
}

const std::vector<Area2DBase*>& Area2DBase::Areas ( void )
{
	return m_areas;
}