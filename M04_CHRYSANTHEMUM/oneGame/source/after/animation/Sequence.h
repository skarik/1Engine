
#ifndef _ANIMATION_SEQUENCE_H_
#define _ANIMATION_SEQUENCE_H_

#include "core/types/types.h"
#include "core-ext/animation/CAnimation.h"
//#include "../../CCharacterModel.h"

#include <string>
#include <vector>

class CCharacterModel;

namespace Animation
{
	enum eSequenceActionType
	{
		SEQ_ACTION_INVALID = 0,
		// The value is an animation name to play.
		SEQ_ACTION_ANIMATION,
		// The value is a set of Lua code. There are very specific in/out variables that pertain to Lua code.
		SEQ_ACTION_LUA,
		// The value marks the end of the sequence
		SEQ_ACTION_END,
		// The value is a new morph blend target. The key contains the target morphs and the new weights.
		SEQ_ACTION_MORPH
	};
	struct SequenceAction
	{
		Real				m_time;
		eSequenceActionType	m_type;
		std::string			m_value;
		std::string			m_subvalue;
	};
	class Sequence
	{
	public:
		Sequence ( void );
		~Sequence( void );

		void Reset ( void );
		void Update ( void );
		void Play ( void );

		// Returns false on read error
		bool LoadFromFile ( const char* filename );
	public:
		CCharacterModel*	m_target;

		std::string	m_name;

		uchar		m_layer;	// 0 for interruptible. 1 for uninterruptible.
		Real		m_time;
		bool		m_loop;
		
		bool		isPlaying;

	private:
		std::vector<SequenceAction>	m_keys;

		std::string	m_current_animation;
		Real		m_current_animation_blend;

	};
};

#endif//_ANIMATION_SEQUENCE_H_