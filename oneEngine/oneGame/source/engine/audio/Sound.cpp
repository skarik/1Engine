#include "core-ext/transform/Transform.h"
#include "audio/Source.h"
#include "engine/audio/Sound.h"

// Define static sound list
std::vector<engine::Sound*>	engine::Sound::soundList;

engine::Sound::Sound ( void ) : CGameBehavior()
{
	mySource		= NULL;
	deleteWhenDone	= false;
	position		= Vector3f();
	velocity		= Vector3f();

	source_position	= NULL;

	soundList.push_back( this );
}

engine::Sound::~Sound ( void )
{
	if ( mySource )
	{
		mySource->Destroy();
	}
	mySource = NULL;

	auto findResult = find( soundList.begin(), soundList.end(), this );
	if ( findResult != soundList.end() )
	{
		soundList.erase( findResult );
	}
}

void engine::Sound::Update ( void )
{
	if ( !mySource ) {
		return;
	}

	if ( source_position ) {
		mySource->state.position = position + *source_position;
		mySource->state.velocity = velocity;
	}
	else {
		mySource->state.position = position;
		mySource->state.velocity = velocity;
	}

	if ( deleteWhenDone ) {
		if ( mySource->Played() ) {
			DeleteObject( this );
		}
	}
}
void engine::Sound::Play ( void )
{
	mySource->Play();
}
void engine::Sound::Stop ( void )
{
	mySource->Stop();
}
void engine::Sound::SetLooped ( bool b_loop )
{
	mySource->state.looped = b_loop;
}

void engine::Sound::SetGain ( float f_gain )
{
	mySource->state.gain = f_gain;
}
void engine::Sound::SetPitch ( float f_pitch )
{
	mySource->state.pitch = f_pitch;
}

const std::vector<engine::Sound*> & engine::Sound::GetSoundList ( void )
{
	return soundList;
}