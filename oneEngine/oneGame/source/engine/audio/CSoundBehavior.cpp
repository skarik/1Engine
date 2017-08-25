
#include "CSoundBehavior.h"
#include "audio/CAudioSource.h"
#include "core-ext/transform/Transform.h"

using std::vector;

// Define static sound list
vector<CSoundBehavior*>	CSoundBehavior::soundList;

CSoundBehavior::CSoundBehavior ( void ) : CGameBehavior()
{
	// Initialize default values
	mySource		= NULL;
	deleteWhenDone	= false;
	position		= Vector3d();
	velocity		= Vector3d();

	source_position	= NULL;

	soundList.push_back( this );
}

CSoundBehavior::~CSoundBehavior ( void )
{
	if ( mySource ) {
		delete mySource;
	}
	mySource = NULL;

	vector<CSoundBehavior*>::iterator findResult;
	findResult = find( soundList.begin(), soundList.end(), this );
	if ( findResult != soundList.end() ) {
		soundList.erase( findResult );
	}
}

void CSoundBehavior::Update ( void )
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
void CSoundBehavior::Play ( void )
{
	mySource->Play();
}
void CSoundBehavior::Stop ( void )
{
	mySource->Stop();
}
void CSoundBehavior::SetLooped ( bool b_loop )
{
	mySource->options.looped = b_loop;
}

void CSoundBehavior::SetGain ( float f_gain )
{
	mySource->options.gain = f_gain;
	mySource->Update();
}
void CSoundBehavior::SetPitch ( float f_pitch )
{
	mySource->options.pitch = f_pitch;
	mySource->Update();
}

const std::vector<CSoundBehavior*> & CSoundBehavior::GetSoundList ( void )
{
	return soundList;
}