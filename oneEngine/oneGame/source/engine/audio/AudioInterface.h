//===============================================================================================//
//
//		class engine::AudioInterface
// 
// Interface for controlling the Audio module. Provides several ease-of-use functions.
//
//===============================================================================================//
#ifndef ENGINE_AUDIO_INTERFACE_H_
#define ENGINE_AUDIO_INTERFACE_H_

#include <string>
#include <unordered_map>

#include "core/containers/arstring.h"
#include "audio/types/AudioStructs.h"

// Often needs to be controlled directly by listeners.
#include "audio/Listener.h"
// Often needs to be controlled directly when calling PlaySound().
#include "engine/audio/Sound.h" 

namespace audio
{
	class Listener;
	class Source;
	class Buffer;
	class Manager;
}

namespace engine
{
	class AudioInterface
	{
	public:
		ENGINE_API audio::Listener* CreateListener ( void );

		//	PlaySound( soundscript ) : Creates a sound behavior from given soundscript.
		// Sound behaviors are engine wrappers for audio::Source.
		ENGINE_API engine::Sound* PlaySound ( const char* soundscriptName );

		ENGINE_API audio::Source* PlayWaveFile ( const char* filename );
		ENGINE_API audio::Source* LoopMusicFile ( const char* filename );

		ENGINE_API std::vector<audio::Source*> GetCurrentSources ( void );

	private:
		std::unordered_map<arstring128, audio::arSoundScript> scriptList;
		std::unordered_map<arstring128, audio::arSoundIndex>  scriptFileIndex;

	private:
		void BuildIndexMap ( void );
		void LoadEntry ( const char* soundName, const audio::arSoundIndex& indexInfo );
		void EditSoundScript ( audio::arSoundScript& script, const std::string& key, const std::string& value );

		//	OpenSoundscriptListing( set ) : Opens a soundscript listing.
		// Searches directories for a proper soundscript listing.
		// Result should be closed with fclose().
		FILE*	OpenSoundscriptListing( audio::eSoundScriptSet set );
	};

	ENGINE_API extern AudioInterface Audio;
}


#endif//ENGINE_AUDIO_INTERFACE_H_