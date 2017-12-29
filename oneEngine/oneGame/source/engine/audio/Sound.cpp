#include "core-ext/transform/Transform.h"
#include "audio/Source.h"
#include "engine/audio/Sound.h"

// Define static sound list
std::vector<engine::Sound*>	engine::Sound::soundList;

engine::Sound::Sound ( void ) : CGameBehavior()
{
	mySource		= NULL;
	deleteWhenDone	= false;
	position		= Vector3d();
	velocity		= Vector3d();

	source_position	= NULL;

	soundList.push_back( this );
}

engine::Sound::~Sound ( void )
{
	if ( mySource ) {
		delete mySource;
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
		mySource->position = position + *source_position;
		mySource->velocity = velocity;
	}
	else {
		mySource->position = position;
		mySource->velocity = velocity;
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
	mySource->options.looped = b_loop;
}

void engine::Sound::SetGain ( float f_gain )
{
	mySource->options.gain = f_gain;
	mySource->Update();
}
void engine::Sound::SetPitch ( float f_pitch )
{
	mySource->options.pitch = f_pitch;
	mySource->Update();
}

const std::vector<engine::Sound*> & engine::Sound::GetSoundList ( void )
{
	return soundList;
}