//
//	CAnimAction.h
// Animation action information.
//
#ifndef _C_ANIM_ACTION_H_
#define _C_ANIM_ACTION_H_

// Includes
#include "core/types/types.h"
#include "core/types/float.h"
#include "core/containers/arstring.h"
#include "core-ext/animation/CAnimationEvents.h"
#include <vector>

class CAnimation;
class CAnimationSet;

//	class CAnimAction
// Storage class for animation states.
// Each "action" represents the state for a single animation.
class CAnimAction
{
public:
	enum tagType_t : uint8_t
	{
		TAG_NORMAL,
		TAG_ITEM,
		TAG_FIDGET
	};
	enum endType_t : uint8_t
	{
		// 0 is for default behavior (set weight to zero and isPlaying to false)
		END_STOP_PLAYING		= 0,
		// 1 is for hold end frame
		END_HOLD_END			= 1,
		// 2 is for hold end frame and fade out
		END_HOLD_END_AND_FADE	= 2
	};
public:
	// Is animation playing
	bool	isPlaying;
	// Framerate of playback
	Real	framesPerSecond;
	// Playback timescale
	Real	playSpeed;
	// Animation blending weight
	Real	weight;
	// Current frame of the animation
	Real	frame;
	// Last frame's frame of the animation
	Real	previousFrame;
	// Ending automatic fade speed
	Real	autoFadeSpeed;
	// Animation layer
	unsigned char	layer;
	// Sync track
	unsigned char	sync_track;
	// Prop overriding information
	unsigned char	prop_override [4];
	// Does animation loop
	bool	loop;
	// Is animation additive
	bool	additive;
	// Reset animation to initial state when not playing. (defaults true)
	bool	reset_on_stop;
	// Ending behavior for when loop is false.
	endType_t	end_behavior;
	// Tag for identification and IK settings
	tagType_t 	tag;
	// Motion extrapolation settings
	bool	extrapolateMotion [3];
	bool	enableMotionExtrapolation [3];

	// Constructor
	CORE_API CAnimAction ( const char* name );
	CORE_API CAnimAction ( void );

	// == Setters ==
	CORE_API void SetRange ( Real fStart, Real fEnd );

	// == Modders ==
	CORE_API void Reset ( void );
	CORE_API void Play ( const Real n_deltaTime, const Real n_playSpeed=1.0F, const Real n_blendTime=0.0F );
	CORE_API void Stop ( void );
	//void Sample ( CAnimationSet*, std::vector<XTransform> const& );

		// Vertex skinned models ONLY!
		// Searches for the given transform in the skeleton and adds it to the mix list.
	//void AddMixingTransform ( string const&, bool );
	CORE_API void AddMixingTransform ( const uint32_t skippedIndex );
	
	// == Updater ==
	// Must be thread-safe
	CORE_API void Update ( const Real n_deltaTime, const Real n_frameOverride );

	// == Getters ==
	const arstring128& GetName ( void ) const
	{
		return actionName;
	};
	std::vector<int16_t>* GetMixingList ( void ) {
		return &mixingList;
	}

	inline Real GetStart ( void ) {
		return start;
	}
	inline Real GetEnd ( void ) {
		return end;
	}
	inline Real GetLength ( void ) {
		return length;
	}
	inline Real Length ( void ) {
		return length;
	}
	inline Real Percent ( void ) {
		return frame / length;
	}
	inline int8_t SourceSet ( void ) {
		return sampleSource;
	}
	inline int8_t SourceMapping ( void ) {
		return sampleMapping;
	}
	inline uint16_t Index ( void ) {
		return index;
	}

	void AddEvent ( const animation::ActionEvent & new_event ) {
		eventList.push_back( new_event );
	}
	int GetEvent ( const animation::eAnimSystemEvent & new_event ) {
		//auto result = find( eventList.begin(), eventList.end(), new_event );
		auto result = eventList.begin();
		while ( result != eventList.end() ) {
			if ( result->type == new_event ) {
				break;
			}
			else {
				++result;
			}
		}
		if ( result != eventList.end() ) {
			return (int)(result->frame);
		}
		else {
			return -1;
		}
	}

	void SetMasterState ( const bool synctothis ) {
		sync_master = synctothis;
	}
	void SetEventsState ( const bool enableevents ) {
		events_enabled = enableevents;
	}

private:
	friend CAnimation;
	friend CAnimationSet;

	arstring128	actionName;
	int8_t sampleSource;
	int8_t sampleMapping;
	// Animation index in the sample source
	uint16_t		index;

	CAnimation*		owner;

	// First frame of the animation
	Real			start;
	// Last frame of the animation
	Real			end;
	// Length of the animation, from beginning to end
	Real			length;

	// Is this a mirrored action?
	bool			mirrored;

	// is this the master?
	bool			sync_master;
	// are events enabled?
	bool			events_enabled;

	// Event list for this action to pass up the animation chain
	std::vector<animation::ActionEvent>	eventList;
	// Mixing list of indices to actually sample this action into
	std::vector<int16_t>		mixingList;
};

#endif