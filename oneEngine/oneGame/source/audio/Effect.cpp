#include "Effect.h"

#include "audio/Manager.h"
#include "audio/mixing/Mixer.h"

audio::Effect::Effect( MixChannel targetChannel )
{
	auto auc = getValidManager();

	// Create the source and perform initialization
	auc->AddEffect( this, targetChannel );
}

audio::Effect::~Effect ( void )
{
	;
}

void audio::Effect::Destroy ( void )
{
	queue_destruction = true;
}
