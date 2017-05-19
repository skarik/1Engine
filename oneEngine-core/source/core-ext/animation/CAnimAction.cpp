
#include "core/time.h"

#include "CAnimation.h"
#include "CAnimAction.h"

// constructor
CAnimAction::CAnimAction( void )
	: weight(0), autoFadeSpeed(4.0F), isPlaying(false), end_behavior(END_STOP_PLAYING), reset_on_stop(true), mirrored(false), tag(TAG_NORMAL),
	sampleSource(0), sampleMapping(0),
	owner(NULL)
{
	actionName = "default";
	layer = 0;
	sync_track = 0;
	additive = false;
	prop_override[0] = false;
	prop_override[1] = false;
	prop_override[2] = false;
	prop_override[3] = false;
	sync_master = false;
	events_enabled = false;

	enableMotionExtrapolation[0] = true;
	enableMotionExtrapolation[1] = true;
	enableMotionExtrapolation[2] = true;
}
CAnimAction::CAnimAction( const char* name )
	: weight(0), autoFadeSpeed(4.0F), isPlaying(false), end_behavior(END_STOP_PLAYING), reset_on_stop(true), mirrored(false), tag(TAG_NORMAL),
	sampleSource(0), sampleMapping(0),
	owner(NULL)
{
	actionName = name;
	layer = 0;
	sync_track = 0;
	additive = false;
	prop_override[0] = false;
	prop_override[1] = false;
	prop_override[2] = false;
	prop_override[3] = false;
	sync_master = false;
	events_enabled = false;

	enableMotionExtrapolation[0] = true;
	enableMotionExtrapolation[1] = true;
	enableMotionExtrapolation[2] = true;
}


// sample
/*void CAnimAction::Sample ( CAnimationSet* pAnimSet, std::vector<XTransform> const& animRefs )
{
	// Loop through all the animations, sample at the current time
	Real currentTime;
	currentTime = start+frame;

#	ifdef _ENGINE_DEBUG
	if ( animRefs.size() > pAnimSet->animMap.size() )
	{
		throw core::CorruptedDataException();
	}
#	endif
	if ( mixingList.size() == 0 )
	{
		for ( unsigned int i = 0; i < animRefs.size(); i++ )
		{
			pAnimSet->animMap[i]->SampleAt( currentTime, weight, additive );
		}
	}
	else
	{
		for ( unsigned int i = 0; i < mixingList.size(); ++i )
		{
			pAnimSet->animMap[mixingList[i]]->SampleAt( currentTime, weight, additive );
		}
	}
}*/

// Update frame values
void CAnimAction::Update ( const Real n_deltaTime, const Real n_frameOverride )
{
	if ( isPlaying )
	{
		previousFrame = frame;
		// Syncing is controlled by master
		if ( !sync_track || (sync_track && sync_master) )
		{
			frame += framesPerSecond*playSpeed*n_deltaTime;	
		}
		else
		{	// Synced by animation object.
			frame = n_frameOverride;
		}
		// Check for event
		if ( events_enabled )
		{
			if ((( !sync_track )||( sync_track && sync_master ))&&( weight > 0.04f ))
			{
				for ( unsigned int i = 0; i < eventList.size(); ++i )
				{
					if (( previousFrame <= eventList[i].frame )&&( eventList[i].frame < frame ))
					{
						owner->PushFrameEvent( eventList[i] );
						//std::cout << "EVENT: " << eventList[i].type << " L" << (int)this->layer << " " << this->actionName << " w=" << this->weight << std::endl;
					}
				}
			}
		}
		// Do limiting
		if ( frame >= length )
		{
			if ( loop )
			{
				if ( length < framesPerSecond*playSpeed*n_deltaTime ) {
					frame = 0;
				}
				else {
					frame -= length;
				}
				// Check for event
				if ( events_enabled )
				{
					if ((( !sync_track )||( sync_track && sync_master ))&&( weight > 0.04f ))
					{
						for ( unsigned int i = 0; i < eventList.size(); ++i )
						{
							if (( 0 <= eventList[i].frame )&&( eventList[i].frame < frame ))
							{
								owner->PushFrameEvent( eventList[i] );
								//std::cout << "EVENT: " << eventList[i].type << " L" << (int)this->layer << " " << this->actionName << " w=" << this->weight << std::endl;
							}
						}
					}
				}
			}
			else
			{
				isPlaying = false;
				frame = length; // set the frame to the last frame
				if ( end_behavior == END_STOP_PLAYING ) {
					weight = 0.0f;
				}
				else if ( end_behavior == END_HOLD_END || end_behavior == END_HOLD_END_AND_FADE ) {
					//weight = 1.0f; // Do not modify weights at the end!
				}
			}
		}
	}
	else
	{
		// Do end behaviors
		if ( end_behavior == END_HOLD_END_AND_FADE )
		{
			weight -= autoFadeSpeed * n_deltaTime;
			if ( weight <= 0 ) {
				weight = 0;
			}
		}
	}
}

// Set the range
void CAnimAction::SetRange( Real fStart, Real fEnd )
{
	if ( fStart <= fEnd )
	{
		start = fStart;
		end = fEnd;
	}
	else
	{
		start = fEnd;
		end = fStart;
	}

	if ( end == start )
		end += 1.0f;

	//start += 1.0f;
	//end += 1.0f;
	//end -= 2.0f;
	end -= 1.0f; // This is done to prevent fp imprecision in the animations. TODO: FIx later!
	length = (Real)((end-start) - FTYPE_PRECISION);
}

// Play the action
void CAnimAction::Play ( const Real n_deltaTime, const Real n_playSpeed, const Real n_blendTime )
{
	if ( !owner )
	{
		throw core::InvalidInstantiationException();
		debug::Console->PrintError( "Bad animation on this object (missing owner)!\n" );
	}
	// No blend time, so playing the animation instantly
	if ( n_blendTime <= FTYPE_PRECISION )
	{
		// Go through all the other stuff
		for ( auto it = owner->mAnimations.begin(); it != owner->mAnimations.end(); it++ )
		{
			if ( &(it->second) != (this) )
			{
				// Set weight to zero if on the same layer
				if ( it->second.layer == layer )
				{
					// In addition to layer checking, check for overlay checks
					if ( it->second.mixingList.empty() ) {
						it->second.isPlaying = false;
						it->second.weight = 0;
					}
					else {
						bool overlays = false;
						for ( auto ml = mixingList.begin(); ml != mixingList.end(); ++ml )
						{
							auto find_ml = find( it->second.mixingList.begin(), it->second.mixingList.end(), *ml );
							if ( find_ml != it->second.mixingList.end() ) {
								overlays = true;
								break;
							}
						}
						if ( overlays ) {
							it->second.isPlaying = false;
							it->second.weight = 0;
						}
					}
				}
			}
		}
		
		// If this isn't playing, set frame to zero
		if ( reset_on_stop && !isPlaying )
		{
			frame = 0;
		}

		// Set the weight and the playback speed
		playSpeed = n_playSpeed;
		weight = 1.0f;

		// Set now is playing
		isPlaying = true;
	}
	// Given a blend time, fade in animation over time
	else
	{
		Real weightIncrement = n_deltaTime / n_blendTime;

		// Go through all the other stuff
		for ( auto it = owner->mAnimations.begin(); it != owner->mAnimations.end(); it++ )
		{
			if ( &(it->second) != (this) )
			{
				// Move other animation's weight to zero if on the same layer
				if (( it->second.layer == layer )&&( it->second.weight > 0 ))
				{
					it->second.weight -= weightIncrement;
					// When weight is smaller than zero, no long play animation
					if ( it->second.weight < 0 )
					{
						it->second.weight = 0;
						it->second.isPlaying = false;
					}
				}
			}
		}

		// If this isn't playing, set frame to zero
		if ( reset_on_stop && !isPlaying )
		{
			frame = 0;
		}

		// Set the weight and the playback speed
		playSpeed = n_playSpeed;
		weight += weightIncrement;
		if ( weight > 1.0F )
		{
			weight = 1.0F;
		}
		
		// Set that now is playing
		isPlaying = true;
	}
}

void CAnimAction::Stop ( void )
{
	Reset();
}

// Reset the action
void CAnimAction::Reset ( void )
{
	isPlaying = false;
	playSpeed = 1.0f;
	weight = 0.0f;
	frame = 0.0f;
}


