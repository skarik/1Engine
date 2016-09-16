
#include "CAfterCharacterAnimator.h"
#include "after/entities/character/CMccCharacterModel.h"
#include "after/states/CRacialStats.h"

#include "after/interfaces/CSpeechTrie.h"

// Speech
void CAfterCharacterAnimator::DoSpeech ( const NPC::eGeneralSpeechType& speechType )
{
	static CSpeechTrie* speechTrie = NULL;
	if ( speechTrie == NULL )
	{
		speechTrie = new CSpeechTrie();
		speechTrie->SetKey( NPC::SpeechType_Jump,			CGEND_FEMALE,	CRACE_FLUXXOR,	"Speech.Fluxxor.Tu.Jump" );
		speechTrie->SetKey( NPC::SpeechType_JumpLand,		CGEND_FEMALE,	CRACE_FLUXXOR,	"Speech.Fluxxor.Tu.JumpLand" );
		speechTrie->SetKey( NPC::SpeechType_Hurt,			CGEND_FEMALE,	CRACE_FLUXXOR,	"Speech.Fluxxor.Tu.Hurt" );
		speechTrie->SetKey( NPC::SpeechType_OutOfBreath,	CGEND_FEMALE,	CRACE_FLUXXOR,	"Speech.Fluxxor.Tu.Huff" );
		speechTrie->SetKey( NPC::SpeechType_GruntYah,		CGEND_FEMALE,	CRACE_FLUXXOR,	"Speech.Fluxxor.Tu.GruntYa" );

		speechTrie->SetKey( NPC::SpeechType_Jump,			CGEND_MALE,		CRACE_HUMAN,	"Speech.Human.Bn.Jump" );
		speechTrie->SetKey( NPC::SpeechType_JumpLand,		CGEND_MALE,		CRACE_HUMAN,	"Speech.Human.Bn.JumpLand" );
		speechTrie->SetKey( NPC::SpeechType_OutOfBreath,	CGEND_MALE,		CRACE_HUMAN,	"Speech.Human.Bn.Huff" );

		speechTrie->SetKey( NPC::SpeechType_Jump,			CGEND_FEMALE,	CRACE_KITTEN,	"Speech.Mayang.Av.JumpF" );
		speechTrie->SetKey( NPC::SpeechType_JumpLand,		CGEND_FEMALE,	CRACE_KITTEN,	"Speech.Mayang.Av.Land" );
		speechTrie->SetKey( NPC::SpeechType_JumpLandHard,	CGEND_FEMALE,	CRACE_KITTEN,	"Speech.Mayang.Av.LandHard" );
		speechTrie->SetKey( NPC::SpeechType_Hurt,			CGEND_FEMALE,	CRACE_KITTEN,	"Speech.Mayang.Av.Hurt" );
		speechTrie->SetKey( NPC::SpeechType_OutOfBreath,	CGEND_FEMALE,	CRACE_KITTEN,	"Speech.Mayang.Av.Huff" );
		speechTrie->SetKey( NPC::SpeechType_Kiai,			CGEND_FEMALE,	CRACE_KITTEN,	"Speech.Mayang.Av.Kiai" );

		speechTrie->SetKey( NPC::SpeechType_Jump,			CGEND_FEMALE,	CRACE_HUMAN,	"Speech.Mayang.Av.JumpF" );
		speechTrie->SetKey( NPC::SpeechType_JumpLand,		CGEND_FEMALE,	CRACE_HUMAN,	"Speech.Mayang.Av.Land" );
		//speechTrie->SetKey( NPC::SpeechType_JumpLandHard,	CGEND_FEMALE,	CRACE_HUMAN,	"Speech.Mayang.Av.LandHard" );
		speechTrie->SetKey( NPC::SpeechType_Hurt,			CGEND_FEMALE,	CRACE_HUMAN,	"Speech.Mayang.Av.Hurt" );
		speechTrie->SetKey( NPC::SpeechType_OutOfBreath,	CGEND_FEMALE,	CRACE_HUMAN,	"Speech.Mayang.Av.Huff" );
		speechTrie->SetKey( NPC::SpeechType_Kiai,			CGEND_FEMALE,	CRACE_HUMAN,	"Speech.Mayang.Av.Kiai" );
	}

	switch ( speechType )
	{
	case NPC::SpeechType_OutOfBreath:
		if ( Time::CurrentTime() - f_speech_HuffTime > 7.0f ) {
			char* dialogue = speechTrie->GetKey( speechType, m_race_stats->iGender, m_race_stats->iRace );
			if ( dialogue != NULL )
			{
				SpeakDialogue( dialogue );
			}
			f_speech_HuffTime = Time::CurrentTime();
		}
		break;
	case NPC::SpeechType_JumpLandHard:
		{
			char* dialogue = NULL;
			if ( Time::CurrentTime() - f_speech_FallhardTime > 30.0f ) {
				// 30 second cooldown on fallhard line
				dialogue = speechTrie->GetKey( speechType, m_race_stats->iGender, m_race_stats->iRace );
				f_speech_FallhardTime = Time::CurrentTime();
			}
			else {
				// Otherwise, just play a damage sound
				dialogue = speechTrie->GetKey( NPC::SpeechType_Hurt, m_race_stats->iGender, m_race_stats->iRace );
			}
			if ( dialogue != NULL )
			{
				SpeakDialogue( dialogue );
			}
		}
		break;
	default:
		{
			// Play the sound!
			char* dialogue = speechTrie->GetKey( speechType, m_race_stats->iGender, m_race_stats->iRace );
			if ( dialogue != NULL )
			{
				SpeakDialogue( dialogue );
			}
			else
			{
				// No sound? Not a problem. Just don't play it.
			}
		}
		break;
	}
}

// SpeakDialogue makes this character speak dialogue. It takes either a raw path or a sound file
#include "core-ext/system/io/Resources.h"
#include "after/states/model/CLipsyncSequence.h"
#include "engine/audio/CAudioInterface.h"
#include "audio/CAudioSource.h"
#include "engine/audio/CSoundBehavior.h"
void CAfterCharacterAnimator::SpeakDialogue ( const string& soundFile )
{
	Real length;
	string playedFile;
	if ( soundFile.find(".mp3") == string::npos && soundFile.find(".ogg") == string::npos && soundFile.find(".wav") == string::npos )
	{	// Load in a sound, set lipsync with the feedback sound file name
		CSoundBehavior* behavior = Audio.playSound( soundFile.c_str() );
		if ( !behavior ) throw std::exception("Could not play sound");
		playedFile = behavior->filename;
		length = (Real)behavior->mySource->GetSoundLength();

		behavior->position = Vector3d( 0,0,5 );
		behavior->parent = &m_model->transform;

		behavior->Update();
		behavior->RemoveReference();
	}
	else
	{	// Load in the raw sound.
		playedFile = Core::Resources::PathTo( "sounds/" + soundFile );
		CAudioSource* t_source = Audio.PlayWaveFile( playedFile.c_str() );
		if ( !t_source ) throw std::exception("Could not find file");
		length = (Real)t_source->GetSoundLength();
		CSoundBehavior* behavior = new CSoundBehavior;
		behavior->mySource			= t_source;
		behavior->ai_alert_amount	= AudioStructs::AI_SPEECH;
		behavior->channel			= AudioStructs::CHAN_SPEECH;
		behavior->deleteWhenDone	= true;

		behavior->position = Vector3d( 0,0,5 );
		behavior->parent = &m_model->transform;

		behavior->Update();
		behavior->RemoveReference();
	}

	string morphFile = playedFile.substr( 0, playedFile.find_last_of('.') );
	morphFile += ".mph";
	CLipsyncSequence* syncer = ((CMccCharacterModel*)m_model)->GetLipSyncer();
	syncer->Load( morphFile );
	syncer->m_position = 0;
	syncer->m_sequence_length = length;
	((CMccCharacterModel*)m_model)->SetLipsyncPlay();
}