
#ifndef _C_ANIM_ACTION_H_
#define _C_ANIM_ACTION_H_

// Includes
#include "core/types/float.h"
#include "core-ext/animation/CAnimationEvents.h"

#include <string>
using std::string;
#include <vector>

// Class prototype
class CAnimation;
class CAnimationSet;

// Class definition
class CAnimAction
{
public:
	enum eTagType {
		TAG_NORMAL,
		TAG_ITEM,
		TAG_FIDGET
	};
public:
	// Is animation playing
	bool	isPlaying;
	// Framerate of playback
	ftype	framesPerSecond;
	// Playback timescale
	ftype	playSpeed;
	// Animation blending weight
	ftype	weight;
	// Current frame of the animation
	ftype	frame;
	// Last frame's frame of the animation
	ftype	previousFrame;
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
	// 0 is for default behavior (set weight to zero and isPlaying to false)
	// 1 is for hold end frame
	// 2 is for hold end frame and fade out
	unsigned char	end_behavior;
	// Tag for identification and IK settings
	eTagType	tag;
	// Motion extrapolation settings
	bool	extrapolateMotion [3];
	bool	enableMotionExtrapolation [3];
	// Animation index
	unsigned int	index;

	// Constructor
	CAnimAction ( string name );
	CAnimAction ( void );

	// == Setters ==
	void SetRange ( ftype fStart, ftype fEnd );

	// == Modders ==
	void Reset ( void );
	void Play ( ftype fPlaySpeed=1.0f, ftype fBlendTime=0.0f );
	void Stop ( void );
	void Sample ( CAnimationSet*, std::vector<void*> const& );

		// Vertex skinned models ONLY!
		// Searches for the given transform in the skeleton and adds it to the mix list.
	//void AddMixingTransform ( string const&, bool );
	void AddMixingTransform ( const uint32_t skippedIndex );
	
	// == Updater ==
	void Update ( const Real n_deltaTime, const ftype n_frameOverride );

	// == Getters ==
	string GetName ( void ) {
		return actionName;
	};
	/*unsigned int GetIndex ( void ) {
		return index;
	}*/
	/*vector<void*>* GetIgnoreList ( void ) {
		return &ignoreList;
	}*/
	/*vector<int>* GetIgnoreList ( void ) {
		return &ignoreList;
	}*/
	std::vector<int>* GetMixingList ( void ) {
		return &mixingList;
	}

	inline ftype GetStart ( void ) {
		return start;
	}
	inline ftype GetEnd ( void ) {
		return end;
	}
	inline ftype GetLength ( void ) {
		return length;
	}
	inline ftype Length ( void ) {
		return length;
	}

	void AddEvent ( const Animation::ActionEvent & new_event ) {
		eventList.push_back( new_event );
	}
	int GetEvent ( const Animation::eAnimSystemEvent & new_event ) {
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
	string			actionName;
	//vector<void*>	ignoreList;
	//vector<int>		ignoreList;
	std::vector<int>		mixingList;
	ftype			start;
	ftype			end;
	ftype			length;

	bool			mirrored;

	// is this the master?
	bool			sync_master;
	// are events enabled?
	bool			events_enabled;

	friend CAnimation;
	CAnimation*		owner;

	std::vector<Animation::ActionEvent>	eventList;

	ftype			deltaTime;
};

#endif