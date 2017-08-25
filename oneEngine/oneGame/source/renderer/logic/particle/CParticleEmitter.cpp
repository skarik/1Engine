
#include "CParticleEmitter.h"
#include "core/time/time.h"
#include "core/system/io/CSegmentedFile.h"
#include "core-ext/system/io/Resources.h"
#include "core/math/random/Random.h"
#include <sstream>

using namespace std;

// Constructor
CParticleEmitter::CParticleEmitter ( void )
	: CLogicObject()
{
	// Set initial states
	m_particles = NULL;
	m_max_particle_index = 0;
	m_next_particle_index = 0;
	m_particle_count = 0;
	m_particle_array_size = 0;
	
	bHasEmitted = false;
	fEmitCount	= 0.0f;

	// Set default options
	bOneShot = false;
	bSimulateInWorldspace = true;
	bSmoothEmitter = false;

	rfParticlesSpawned.SetRange( 1,2 );
	fMaxParticles = 50.0f;
	
	rfLifetime.SetRange( 1,1 );

	rfStartSize.SetRange( 1,1 );
	rfEndSize.SetRange( 1,1 );

	rfStartAngle.SetRange( 0,0 );
	rfAngularVelocity.SetRange( 0,0 );
	rfAngularAcceleration.SetRange( 0,0 );
	rfAngularDamping.SetRange( 0,0 );

	rvVelocity.SetRange( Vector3d(),Vector3d() );
	rvAcceleration.SetRange( Vector3d(),Vector3d() );
	rvLinearDamping.SetRange( Vector3d(),Vector3d() );

	//vcColors.SetRange( Color(),Color() );

	m_lastPosition = transform.position;
	m_worldVelocity = Vector3d(0,0,0);
	fVelocityScale = 0;
}

CParticleEmitter::~CParticleEmitter ( void )
{
	if ( m_particles != NULL )
	{
		delete [] m_particles;
		m_particles = NULL;
	}
}

// Start up system
void CParticleEmitter::Initialize ( void )
{
	if ( m_particles != NULL && m_particle_array_size != (uint16_t)fMaxParticles )
	{
		delete [] m_particles;
	}
	// Create new array of particles
	m_particle_array_size = bOneShot ? (uint16_t)fMaxParticles : std::min<uint16_t>( (uint16_t)(rfParticlesSpawned.mMaxVal * rfLifetime.mMaxVal * 1.41F), (uint16_t)fMaxParticles );
	m_particles = new CParticle [m_particle_array_size];
	// Zero out their data
	memset( m_particles, 0, sizeof(CParticle) * m_particle_array_size );

	// Reset the other values
	m_max_particle_index = 0;
	m_next_particle_index = 0;
	m_particle_count = 0;
}

// Update
void CParticleEmitter::PreStepSynchronus ( void )
{
	// Update velocity
	m_worldVelocity = m_worldVelocity.lerp( (transform.position-m_lastPosition) / Time::deltaTime, Time::deltaTime*10 );
	m_lastPosition = transform.position;
	// Emit if we have a working particle listing
	if ( m_particles != NULL )
	{
		// Update emit
		if ( !bOneShot )
		{
			Emit();
			bHasEmitted = false;
		}
		else
		{
			if ( !bHasEmitted )
			{
				Emit();
				bHasEmitted = true;
			}
		}
	}
}

// Emit Particles
void CParticleEmitter::Emit ( void )
{
	if ( m_particle_count < m_particle_array_size )
	{
		if ( !bOneShot )
		{
			Real fReleaseCount = 0;
			Real fTargetReleaseCount = rfParticlesSpawned.GetRandom() * CTime::deltaTime;

			fEmitCount += fTargetReleaseCount;
			fEmitCount = std::min<Real>( fEmitCount, (Real)(m_particle_array_size - m_particle_count) - 1.0F );
			if ( fEmitCount >= 1.0f )
			{
				while ( fReleaseCount < fEmitCount )
				{
					fReleaseCount += 1.0f;

					Vector3d targetPosition( random_range( -vEmitterSize.x, vEmitterSize.x ), random_range( -vEmitterSize.y, vEmitterSize.y ), random_range( -vEmitterSize.z, vEmitterSize.z ) );
					CreateParticle ( targetPosition );
				}
				fEmitCount -= 1.0f;
			}
		}
		else
		{
			Real fReleaseCount = 0;
			Real fTargetReleaseCount = rfParticlesSpawned.GetRandom();

			while (( fReleaseCount < fTargetReleaseCount )&&( fReleaseCount < fMaxParticles ))
			{
				fReleaseCount += 1.0f;

				Vector3d targetPosition( random_range( -vEmitterSize.x, vEmitterSize.x ), random_range( -vEmitterSize.y, vEmitterSize.y ), random_range( -vEmitterSize.z, vEmitterSize.z ) );
				CreateParticle ( targetPosition );
			}
			fEmitCount = 0.0f;
		}
	}
}

// Create new particle and add to internal particle list
void CParticleEmitter::CreateParticle ( const Vector3d & vInPosition )
{
	if ( m_next_particle_index >= m_particle_array_size )
	{	// Need to do a quick check to fix this edge case
		throw core::YouSuckException();
	}
	CParticle& newParticle = m_particles[m_next_particle_index];

	// Set the new values
	if ( !bSmoothEmitter )
	{
		newParticle.fStartLife	= rfLifetime.GetRandom();
		newParticle.fLife		= newParticle.fStartLife;

		newParticle.fStartSize	= rfStartSize.GetRandom();
		newParticle.fEndSize	= rfEndSize.GetRandom();
		newParticle.fSize		= newParticle.fStartSize;

		newParticle.fAngle			= rfStartAngle.GetRandom();
		newParticle.fAngleVelocity	= rfAngularVelocity.GetRandom();
		newParticle.fAngleAcceleration = rfAngularAcceleration.GetRandom();
		newParticle.fAngleDamping	= rfAngularDamping.GetRandom();

		newParticle.vPosition		= (bSimulateInWorldspace)?(vInPosition + transform.position):vInPosition;
		newParticle.vVelocity		= transform.rotation*rvVelocity.GetRandom() + m_worldVelocity*fVelocityScale;
		newParticle.vAcceleration	= rvAcceleration.GetRandom();
		newParticle.vLinearDamping	= rvLinearDamping.GetRandom();
	}
	else
	{
		newParticle.fStartLife	= rfLifetime.GetNext();
		newParticle.fLife		= newParticle.fStartLife;

		newParticle.fStartSize	= rfStartSize.GetNext();
		newParticle.fEndSize	= rfEndSize.GetNext();
		newParticle.fSize		= newParticle.fStartSize;

		newParticle.fAngle			= rfStartAngle.GetNext();
		newParticle.fAngleVelocity	= rfAngularVelocity.GetNext();
		newParticle.fAngleAcceleration = rfAngularAcceleration.GetNext();
		newParticle.fAngleDamping	= rfAngularDamping.GetNext();

		newParticle.vPosition		= (bSimulateInWorldspace)?(vInPosition + transform.position):vInPosition;
		newParticle.vVelocity		= transform.rotation*rvVelocity.GetNext() + m_worldVelocity*fVelocityScale;
		newParticle.vAcceleration	= rvAcceleration.GetNext();
		newParticle.vLinearDamping	= rvLinearDamping.GetNext();
	}
	newParticle.alive = true;

	// Increment particle count
	m_particle_count += 1;
	m_next_particle_index += 1;

	// Have we hit the end? If so, we need to slide back to fill in all the now-empty spots
	if ( m_next_particle_index >= m_particle_array_size )
	{
		int offset = 0;
		for ( uint i = 0; i < m_next_particle_index; ++i )
		{
			// For each blank particle, we need to increase the offset to pull the particle from
			while ( i+offset < m_next_particle_index && !m_particles[i+offset].alive )
			{
				offset++;
			}
			// If we're pulling from ahead of the array...
			if ( offset != 0 )
			{
				uint i_other = i+offset;
				if ( i_other < m_next_particle_index )
				{	// Pull the next particle
					m_particles[i] = m_particles[i_other];
				}
				else
				{	// Otherwise, it is a dead particle
					m_particles[i].alive = false;
				}
			}
		}
		// Subtract the amount of empty spots from the max size to get the next particle we can use
		m_next_particle_index = m_particle_array_size - offset;
	}

	// Max particle index to loop through will alpways be the same as the max index.
	m_max_particle_index = m_next_particle_index;
}

// Return particle count
unsigned int CParticleEmitter::GetParticleCount ( void ) {
	//return vParticles.size();
	return m_particle_count;
}
// Return if emitted
bool CParticleEmitter::HasEmitted ( void ) {
	return bHasEmitted;
}

// Load stuff
void CParticleEmitter::LoadFromFile ( const string & filename )
{
	// Input
	CSegmentedFile inFile ( core::Resources::PathTo( filename ) );
	inFile.ReadData();

	// First check header
	uint32_t iVersion;
	{
		// Get data
		string str;
		inFile.GetSectionData( string("header"), str );
		stringstream sin ( str, ios_base::binary|ios_base::in|ios_base::out );

		// Read version
		sin.read( (char*)(&iVersion), sizeof(uint32_t) );
	}

	// Load depending on version number
	if ( iVersion == 1 )
	{
		uint32_t	tempInt;
		char		tempChar;

		// Get data
		string str;
		inFile.GetSectionData( string("emitter"), str );
		stringstream sin ( str, ios_base::binary|ios_base::in|ios_base::out );

		// version 1 baseemitter-only loading

		// [bool] bOneShot
		sin.read( (char*)(&tempChar), sizeof(char) );
		bOneShot				= tempChar ? true : false;
		// [bool] bSimulateInWorldspace
		sin.read( (char*)(&tempChar), sizeof(char) );
		bSimulateInWorldspace	= tempChar ? true : false;

		// [ftype,range] rfParticlesSpawned
		sin.read( (char*)(&rfParticlesSpawned.mMinVal), sizeof(ftype) );
		sin.read( (char*)(&rfParticlesSpawned.mMaxVal), sizeof(ftype) );
		// [ftype] fMaxParticles
		sin.read( (char*)(&fMaxParticles), sizeof(ftype) );

		// [ftype,range] rfLifetime
		sin.read( (char*)(&rfLifetime.mMinVal), sizeof(ftype) );
		sin.read( (char*)(&rfLifetime.mMaxVal), sizeof(ftype) );

		// [ftype,range] rfStartSize
		sin.read( (char*)(&rfStartSize.mMinVal), sizeof(ftype) );
		sin.read( (char*)(&rfStartSize.mMaxVal), sizeof(ftype) );
		// [ftype,range] rfEndSize
		sin.read( (char*)(&rfEndSize.mMinVal), sizeof(ftype) );
		sin.read( (char*)(&rfEndSize.mMaxVal), sizeof(ftype) );

		// [ftype,range] rfStartAngle
		sin.read( (char*)(&rfStartAngle.mMinVal), sizeof(ftype) );
		sin.read( (char*)(&rfStartAngle.mMaxVal), sizeof(ftype) );
		// [ftype,range] rfAngularVelocity
		sin.read( (char*)(&rfAngularVelocity.mMinVal), sizeof(ftype) );
		sin.read( (char*)(&rfAngularVelocity.mMaxVal), sizeof(ftype) );
		// [ftype,range] rfAngularAcceleration
		sin.read( (char*)(&rfAngularAcceleration.mMinVal), sizeof(ftype) );
		sin.read( (char*)(&rfAngularAcceleration.mMaxVal), sizeof(ftype) );
		// [ftype,range] rfAngularDamping
		sin.read( (char*)(&rfAngularDamping.mMinVal), sizeof(ftype) );
		sin.read( (char*)(&rfAngularDamping.mMaxVal), sizeof(ftype) );

		// [Vector3d,range] rvVelocity
		sin.read( (char*)(&rvVelocity.mMinVal.x), sizeof(ftype) );
		sin.read( (char*)(&rvVelocity.mMinVal.y), sizeof(ftype) );
		sin.read( (char*)(&rvVelocity.mMinVal.z), sizeof(ftype) );
		sin.read( (char*)(&rvVelocity.mMaxVal.x), sizeof(ftype) );
		sin.read( (char*)(&rvVelocity.mMaxVal.y), sizeof(ftype) );
		sin.read( (char*)(&rvVelocity.mMaxVal.z), sizeof(ftype) );
		// [Vector3d,range] rvAcceleration
		sin.read( (char*)(&rvAcceleration.mMinVal.x), sizeof(ftype) );
		sin.read( (char*)(&rvAcceleration.mMinVal.y), sizeof(ftype) );
		sin.read( (char*)(&rvAcceleration.mMinVal.z), sizeof(ftype) );
		sin.read( (char*)(&rvAcceleration.mMaxVal.x), sizeof(ftype) );
		sin.read( (char*)(&rvAcceleration.mMaxVal.y), sizeof(ftype) );
		sin.read( (char*)(&rvAcceleration.mMaxVal.z), sizeof(ftype) );
		// [Vector3d,range] rvLinearDamping
		sin.read( (char*)(&rvLinearDamping.mMinVal.x), sizeof(ftype) );
		sin.read( (char*)(&rvLinearDamping.mMinVal.y), sizeof(ftype) );
		sin.read( (char*)(&rvLinearDamping.mMinVal.z), sizeof(ftype) );
		sin.read( (char*)(&rvLinearDamping.mMaxVal.x), sizeof(ftype) );
		sin.read( (char*)(&rvLinearDamping.mMaxVal.y), sizeof(ftype) );
		sin.read( (char*)(&rvLinearDamping.mMaxVal.z), sizeof(ftype) );

		// [uint32_t] colorNum
		sin.read( (char*)(&tempInt), sizeof(uint32_t) );
		while ( vcColors.size() != tempInt )
		{
			if ( vcColors.size() < tempInt )
				vcColors.push_back( Color( 1.0f,1.0f,1.0f,0.0f ) );
			if ( vcColors.size() > tempInt )
				vcColors.pop_back();
		}
		// [Colors[colorNum]] vcColors
		for ( uint32_t i = 0; i < tempInt; ++i )
		{
			sin.read( (char*)(&vcColors[i].red), sizeof(ftype) );
			sin.read( (char*)(&vcColors[i].green), sizeof(ftype) );
			sin.read( (char*)(&vcColors[i].blue), sizeof(ftype) );
			sin.read( (char*)(&vcColors[i].alpha), sizeof(ftype) );
		}
	}
	else
	{
		cout << "Wrong particle version loader." << endl;
	}
}

#include "core-ext/system/io/serializer/CBaseSerializer.h"
void	CParticleEmitter::serialize ( Serializer & ser, const uint ver )
{
	ser & name;

	ser & vEmitterSize;

	ser & bOneShot;
	ser & bSimulateInWorldspace;
	ser & bSmoothEmitter;

	ser & rfParticlesSpawned;
	ser & fMaxParticles;

	ser & rfLifetime;

	ser & rfStartSize;
	ser & rfEndSize;

	ser & rfStartAngle;
	ser & rfAngularVelocity;
	ser & rfAngularAcceleration;
	ser & rfAngularDamping;

	ser & rvVelocity;
	ser & rvAcceleration;
	ser & rvLinearDamping;

	ser & vcColors;

	if ( ver < 3 ) return;

	ser & fVelocityScale;

	Initialize();
}