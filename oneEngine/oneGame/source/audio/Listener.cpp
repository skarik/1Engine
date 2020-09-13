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

	// Set initial options
	position = Vector3f();
	position_prev = Vector3f();
	velocity = Vector3f();
	orient_forward = Vector3f::forward;
	orient_up = Vector3f( 0,0,1 );
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