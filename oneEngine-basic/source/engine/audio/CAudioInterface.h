
#ifndef _C_AUDIO_INTERFACE_H_
#define _C_AUDIO_INTERFACE_H_

#include <string>
#include "core/containers/arstring.h"

// Class prototypes
class CAudioListener;
class CAudioSource;
class CAudioSound;
class CAudioManager;
class CSoundBehavior;

// Includes
#include "audio/AudioStructs.h"
#include "audio/CAudioListener.h"	// Should be the only thing ever needed
#include "CSoundBehavior.h"

#if __cplusplus > 199711L
	#include <unordered_map>
	using std::unordered_map;
#else
	#include <unordered_map>
	using std::tr1::unordered_map;
#endif

class CAudioInterface
{
public:
	ENGINE_API CAudioListener* CreateListener ( void );

	ENGINE_API CAudioSource* PlayWaveFile ( const char* );
	ENGINE_API CAudioSource* LoopMusicFile ( const char* );

	ENGINE_API CSoundBehavior* playSound ( const char* soundName );

	ENGINE_API std::vector<CAudioSource*> GetCurrentSources ( void );

private:
	typedef AudioStructs::soundIndex_t soundIndex_t;
	typedef AudioStructs::soundScript_t soundScript_t;
	typedef AudioStructs::soundScript_aialert soundScript_aialert;
	typedef AudioStructs::soundScript_channel soundScript_channel;

	unordered_map<arstring<128>, soundScript_t> scriptList;
	unordered_map<arstring<128>, soundIndex_t>  scriptFileIndex;

private:
	void BuildIndexMap ( void );
	void LoadEntry ( const char* soundName, const soundIndex_t& indexInfo );
	void EditSoundScript ( soundScript_t & script, const std::string& key, const std::string& value );
};

ENGINE_API extern CAudioInterface Audio;

#endif