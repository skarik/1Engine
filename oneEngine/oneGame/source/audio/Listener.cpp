#include "audio/AudioMaster.h"
#include "audio/Listener.h"

audio::Listener::Listener( void )
{
	queue_destruction = false;

	if ( audio::Master::Active() )
	{
		audio::Master::GetCurrent()->AddListener( this );
	}

	// Set initial options
	position = Vector3f();
	position_prev = Vector3f();
	velocity = Vector3f();
	orient_forward = Vector3f::forward;
	orient_up = Vector3f( 0,0,1 );
}

audio::Listener::~Listener ( void )
{
	if ( audio::Master::Active() )
	{
		audio::Master::GetCurrent()->RemoveListener( this );
	}
}

void audio::Listener::Update ( void )
{
	velocity = (position_prev - position);
	position_prev = position;

//#ifndef _AUDIO_FMOD_
//	alListener3f(AL_POSITION, position.x, position.y, position.z);
//	alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
//	//alListener3f(AL_ORIENTATION, orientation.x, orientation.y, orientation.z);
//	float orientation [6];
//	orientation[0] = orient_forward.x;
//	orientation[1] = orient_forward.y;
//	orientation[2] = orient_forward.z;
//	orientation[3] = orient_up.x;
//	orientation[4] = orient_up.y;
//	orientation[5] = orient_up.z;
//	alListenerfv( AL_ORIENTATION, orientation );
//#else
//	FMOD::FMOD_VECTOR t_position;
//	FMOD::FMOD_VECTOR t_velocity;
//	FMOD::FMOD_VECTOR t_orient_forward;
//	FMOD::FMOD_VECTOR t_orient_up;
//
//	t_position.x = position.x;
//	t_position.y = position.y;
//	t_position.z = position.z;
//
//	t_velocity.x = velocity.x;
//	t_velocity.y = velocity.y;
//	t_velocity.z = velocity.z;
//
//	t_orient_forward.x = orient_forward.x;
//	t_orient_forward.y = orient_forward.y;
//	t_orient_forward.z = orient_forward.z;
//
//	t_orient_up.x = orient_up.x;
//	t_orient_up.y = orient_up.y;
//	t_orient_up.z = orient_up.z;
//
//	FMOD::FMOD_System_Set3DListenerAttributes( CAudioMaster::System(), 0, &t_position, &t_velocity, &t_orient_forward, &t_orient_forward );
//#endif
}

void audio::Listener::Destroy ( void )
{
	queue_destruction = true;
}