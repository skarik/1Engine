
#include "CPhysWindBase.h"

//===============================================================================================//
// Wind shape management
//===============================================================================================//

std::vector<CPhysWindBase*>	CPhysWindBase::_wind_listing;
std::vector<CPhysWindBase*>& CPhysWindBase::Listing ( void )
{
	return _wind_listing;
}

CPhysWindBase::CPhysWindBase ( void )
{
	if ( std::find( _wind_listing.begin(), _wind_listing.end(), this ) == _wind_listing.end() ) 
	{
		_wind_listing.push_back( this );
	}
}
CPhysWindBase::~CPhysWindBase ( void )
{
	auto find_result = std::find( _wind_listing.begin(), _wind_listing.end(), this );
	if ( find_result == _wind_listing.end() ) {
		_wind_listing.erase( find_result );
	}
}

//===============================================================================================//
// Havok Wind system that works off of CPhysWindBase
//===============================================================================================//

physWindManager* physWindManager::Instantiate ( void )
{
	return new physWindManager;
}
physWindManager::physWindManager ( void )
{
	;
}
physWindManager::~physWindManager ( void )
{
	;
}
/*void physWindManager::addWind ( CPhysWindBase *newWind )
{
	m_winds.pushBack( newWind );
}*/

void physWindManager::update ( physReal delta )
{
	int i = 0;
	auto m_winds = CPhysWindBase::Listing();
	//while ( i < m_winds.getSize() )
	while ( i < (int)m_winds.size() )
	{
		m_winds[i]->Update( delta );
		if ( m_winds[i]->IsActive() )
		{
			++i;
		}
		else
		{
			//m_winds[i]->removeReference();
			delete m_winds[i];
			//m_winds.removeAt( i );
			m_winds.erase( m_winds.begin() + i );
		}
	}
}
void physWindManager::getWindVector ( const physVector4& pos, physVector4& windOut ) const
{
	windOut.setZero4(); // Reset wind

	Vector3f vWindOut;
	Vector3f vWindValue;
	Vector3f vPos ( pos.getComponent<0>(),pos.getComponent<1>(),pos.getComponent<2>() );
	auto m_winds = CPhysWindBase::Listing();
	//const int numWinds = m_winds.getSize();
	const int numWinds = m_winds.size();
	for ( int i = 0; i < numWinds; ++i )
	{
		m_winds[i]->GetWindVector( vPos, vWindValue );
		vWindOut += vWindValue;
	}

	windOut.set( vWindOut.x, vWindOut.y, vWindOut.z );
}

physWindRegion* physWindManager::InstantiateWindRegion( physAabbPhantom* phantom, const physWind* wind, physReal resistanceFactor, physReal obbFactor )
{
	return new physWindRegion( phantom, wind, resistanceFactor, obbFactor );
}