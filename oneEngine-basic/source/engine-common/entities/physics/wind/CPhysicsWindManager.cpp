
#include "CPhysicsWindManager.h"
#include "core/time/time.h"
#include "physical/physics/wind/CPhysWindBase.h"

CPhysicsWindManager* CPhysicsWindManager::Active = NULL;
CPhysicsWindManager* CPhysicsWindManager::GetActive ( void ) {
	return Active;
}


bool				CPhysicsWindManager::bReady	= false;
physWindManager *	CPhysicsWindManager::m_physWindManager	= NULL;
physWindRegion *	CPhysicsWindManager::m_region			= NULL;
physAabbPhantom*	CPhysicsWindManager::m_phantom			= NULL;
physAabb			CPhysicsWindManager::m_aabb;


CPhysicsWindManager::CPhysicsWindManager ( void )
	: CGameBehavior()
{
	//pCurrentWindManager = this;
	Active = this;

	if ( !bReady )
	{
		// Create phantom
		m_aabb.m_min.set( -90,-90,-90 );
		m_aabb.m_max.set( +90,+90,+90 );
		//m_phantom = new hkpAabbPhantom ( m_aabb );
		m_phantom = (physAabbPhantom*)Physics::CreateAABBPhantom( &m_aabb, this->GetId() );
		//Physics::AddPhantom( m_phantom );
		// Create wind manager
		//m_physWindManager = new physWindManager ();
		m_physWindManager = physWindManager::Instantiate();
		// Create wind region
		//m_region = new hkpWindRegion( m_phantom, m_physWindManager, 0.1f );
		m_region = physWindManager::InstantiateWindRegion( m_phantom, m_physWindManager, 0.1f );
		Physics::AddListener( m_region );

		bReady = true;
	}
}

CPhysicsWindManager::~CPhysicsWindManager ( void )
{
	// TODO: Figure out proper handling of these pointers to prevent havok virtual call issues
	//m_physWindManager->removeReference();
	//m_region->removeReference();
	//m_phantom->removeReference();

	//m_aabb->removeReference();

	//pCurrentWindManager = NULL;
	Active = NULL;
}

void CPhysicsWindManager::FixedUpdate ( void )
{
	//m_phantom->setAabb
	Vector3d vCenter;
	Physics::GetWorldCenter( vCenter );
	//m_aabb = hkAabb ( hkVector4( vCenter.x-90,vCenter.y-90,vCenter.z-90 ), hkVector4( vCenter.x+90,vCenter.y+90,vCenter.z+90 ) );
	m_aabb.m_min.set( vCenter.x-90,vCenter.y-90,vCenter.z-90 );
	m_aabb.m_max.set( vCenter.x+90,vCenter.y+90,vCenter.z+90 );
	//m_phantom->setAabb( m_aabb );
	Physics::SetPhantomAABB( m_phantom, &m_aabb );

	m_physWindManager->update( Time::deltaTime );
}

/*void CPhysicsWindManager::AddWind ( CPhysWindBase* windToAdd )
{
	m_physWindManager->addWind( windToAdd );
}*/