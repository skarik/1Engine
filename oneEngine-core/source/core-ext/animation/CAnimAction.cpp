
#include "CAnimation.h"
#include "CAnimAction.h"
//#include "core/time/time.h"

// constructor
CAnimAction::CAnimAction( void )
	: weight(0), isPlaying(false), end_behavior(0), reset_on_stop(true), mirrored(false), tag(TAG_NORMAL), owner(NULL)
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
	: weight(0), isPlaying(false), end_behavior(0), reset_on_stop(true), mirrored(false), tag(TAG_NORMAL), owner(NULL)
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
void CAnimAction::Sample ( CAnimationSet* pAnimSet, std::vector<void*> const& animRefs )
{
	// Loop through all the animations, sample at the current time
	ftype currentTime;
	currentTime = start+frame;

#	ifdef _ENGINE_DEBUG
	if ( animRefs.size() > pAnimSet->animMap.size() )
	{
		throw Core::CorruptedDataException();
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

	/*vector<CAnimationSet::sCurveReference>::it = pAnimSet->animMap.begin();
	do
	{
		// Check to make sure the current ref isn't in the ignore list

		it++;
	}
	while ( it != pAnimSet->animMap.end() );*/
	/*for ( unsigned int i = 0; i < animRefs.size(); i++ )
	{
		// Check to see current ref isn't in ignore list
		bool ignored = false;
		for ( unsigned int j = 0; j < ignoreList.size(); j++ )
		{
			//if ( ignoreList[j] == animRefs[i] )
			if ( ignoreList[j] == i )
			{
				ignored = true;
				j = ignoreList.size();
			}
		}

		// If not ignored, then sample track
		if ( !ignored )
		{
			pAnimSet->animMap[i]->SampleAt( currentTime, weight, additive );
		}
	}*/
}

// Update frame values
void CAnimAction::Update ( const Real n_deltaTime, const ftype n_frameOverride )
{
	deltaTime = n_deltaTime;
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
				if ( end_behavior == 0 ) {
					weight = 0.0f;
				}
				else if ( end_behavior == 1 || end_behavior == 2 ) {
					//weight = 1.0f; // Do not modify weights at the end!
				}
			}
		}
	}
	else
	{
		// Do end behaviors
		if ( end_behavior == 2 )
		{
			if ( weight > 0 ) {
				weight -= 4.0f * n_deltaTime;
			}
			else {
				weight = 0;
			}
		}
	}
}

// Set the range
void CAnimAction::SetRange( ftype fStart, ftype fEnd )
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
	end -= 1.0f; // This is done to prevent fp imprecision in the animations. FIx later!
	length = (Real)((end-start) - FTYPE_PRECISION);
}

// Play the action
void CAnimAction::Play ( ftype fPlaySpeed, ftype fBlendTime )
{
	if ( !owner ) {
		Debug::Console->PrintError( "Bad animation on this object (missing owner)!\n" );
		return;
	}
	if ( fBlendTime < 0.01f )
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
		playSpeed = fPlaySpeed;
		weight = 1.0f;

		// Set now is playing
		isPlaying = true;
	}
	else // Move weight in slowly
	{
		ftype weightIncrement = deltaTime / fBlendTime;

		// Go through all the other stuff
		for ( auto it = owner->mAnimations.begin(); it != owner->mAnimations.end(); it++ )
		{
			if ( &(it->second) != (this) )
			{
				// Move weight to zero if on the same layer
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
		playSpeed = fPlaySpeed;
		weight += weightIncrement;
		if ( weight > 1 )
		{
			weight = 1;
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


