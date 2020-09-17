#include "audio/Manager.h"
#include "audio/Listener.h"

audio::Listener::Listener( void )
{
	auto auc = getValidManager();

	queue_destruction = false;

	if ( auc->IsActive() )
	{
		auc->AddListener( this );
	}
}

audio::Listener::~Listener ( void )
{
	;
}

void audio::Listener::Update ( void )
{
	velocity = (position_prev - position);
	position_prev = position;
}

void audio::Listener::Destroy ( void )
{
	queue_destruction = true;
}