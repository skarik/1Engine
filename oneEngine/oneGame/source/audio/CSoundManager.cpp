
#include "CAudioMaster.h"
#include "CSoundManager.h"
#include "CAudioSound.h"
#include "CAudioSoundStreamed.h"
#include "core/utils/StringUtils.h"

using std::string;
using std::map;

//CSoundManager SoundManager;
// Global instance
CSoundManager* CSoundManager::Active = NULL;
// Returns current active sound manager. It is created and destroyed by CAudioMaster.
CSoundManager* CSoundManager::GetActive ( void )
{
	return Active;
}

CAudioSound*	CSoundManager::GetSound ( const char* soundName, const int n_positional )
{
	if ( !CAudioMaster::Active() )
		return NULL;	// Do not load buffers if the device has failed

	// First look for it
	map<arstring<128>,CAudioSound*>::iterator it;
	arstring<128> ar_soundName (soundName);

	it = soundmap.find( ar_soundName );
	// It's not here
	if ( it == soundmap.end() )
	{
		std::cout << "New sound reference: " << soundName << std::endl;

		//try
		//{
			// So create a new reference
			//  (choose between streamed and buffered based on extension)
		string sFileExtension = StringUtils::ToLower( StringUtils::GetFileExtension( soundName ) );
		if ( sFileExtension == "ogg" || sFileExtension == "mp3" || sFileExtension == "mp2" )
		{
			if ( n_positional == -1 ) {
				soundmap[ar_soundName] = new CAudioSoundStreamed( soundName, false );
			}
			else {
				soundmap[ar_soundName] = new CAudioSoundStreamed( soundName, n_positional );
			}
		}
		else
		{
			if ( n_positional == -1 ) {
				soundmap[ar_soundName] = new CAudioSound( soundName, true );
			}
			else {
				soundmap[ar_soundName] = new CAudioSound( soundName, n_positional );
			}
		}

		return soundmap[ar_soundName];
		/*}
		catch (...)
		{
			return NULL;
		}*/
	}
	else
	{
		// Return found value
		return it->second;
	}
}