
#ifndef _AUDIO_STRUCTS_H_
#define _AUDIO_STRUCTS_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"

namespace AudioStructs
{
	enum soundScript_channel
	{
		CHAN_DEFAULT = 0,
		CHAN_PHYSICS,
		CHAN_HEAVY,
		CHAN_SPEECH,
		CHAN_BACKGROUND,
		CHAN_MUSIC
	};
	enum soundScript_aialert
	{
		AI_IGNORE = 0,
		AI_SMALL,
		AI_SPEECH,
		AI_NOTICE
	};
	struct soundScript_t
	{
		soundScript_aialert	ai_alert_amount;
		float				pitch;
		float				attenuation;
		float				gain;
		soundScript_channel	channel;
		char				loop;
		//char				sounds [256][4];
		///std::array<char[256],4>			sounds;
		arstring<256>		sounds [4];
		char				sound_usage [4];
		char				count;
	};
	struct soundIndex_t
	{
		uint32_t pos;
		char	 set;
	};
}
namespace AudioInfo = AudioStructs; // namespace alias because people keep bitching at my naming scheme

#endif//_AUDIO_STRUCTS_H_