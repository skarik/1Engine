
#ifndef _C_ACTOR_DIALOGUE_H_
#define _C_ACTOR_DIALOGUE_H_

namespace NPC {
	enum eGeneralSpeechType
	{
		SpeechType_Jump = 0,
		SpeechType_JumpLand,
		SpeechType_JumpLandHard,
		SpeechType_Hurt,
		SpeechType_HurtHeavy,
		SpeechType_OutOfBreath,

		SpeechType_Kiai,

		SpeechType_GruntYah,
		
		SpeechType_Yes,
		SpeechType_No,
		SpeechType_Maybe,

		SpeechType_Cheer,


		SpeechType_USER
	};
};

#endif//_C_ACTOR_DIALOGUE_H_