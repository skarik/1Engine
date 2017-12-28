#ifndef AUDIO_TYPE_STRUCTS_H_
#define AUDIO_TYPE_STRUCTS_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"

namespace audio
{
	enum eSoundScriptChannel
	{
		kChannelDefault = 0,
		kChannelPhysics,
		kChannelHeavy,
		kChannelSpeech,
		kChannelBackground,
		kChannelMusic,
	};
	enum eSoundScriptAIAlert
	{
		kAIAlertIgnore = 0,
		kAIAlertSmall,
		kAIAlertSpeech,
		kAIAlertNotice,
	};
	struct arSoundScript
	{
		eSoundScriptChannel	ai_alert_amount;
		float				pitch;
		float				attenuation;
		float				gain;
		eSoundScriptAIAlert	channel;
		char				loop;
		arstring256			sounds [4];
		char				sound_usage [4];
		char				count;
	};
	struct arSoundIndex
	{
		uint32_t pos;
		char	 set;
	};
}

#endif//AUDIO_TYPE_STRUCTS_H_