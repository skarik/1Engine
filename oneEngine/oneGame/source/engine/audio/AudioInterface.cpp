#include "core/debug/console.h"
#include "core-ext/system/io/Resources.h"

#include "audio/Manager.h"
#include "audio/Listener.h"
#include "audio/Source.h"
#include "audio/Buffer.h"
#include "audio/BufferManager.h"

#include "AudioInterface.h"

#include <stdio.h>
#include <ctype.h>

engine::AudioInterface engine::Audio;

std::vector<audio::Source*> engine::AudioInterface::GetCurrentSources ( void )
{
	return *audio::Master::GetCurrent()->GetSources();
}

audio::Listener* engine::AudioInterface::CreateListener ( void )
{
	return new audio::Listener();
}

audio::Source* engine::AudioInterface::PlayWaveFile ( const char* filename )
{
	// First create the sound
	audio::Buffer* newSound = audio::BufferManager::Active()->GetSound( filename );
	if ( !newSound ) {
		return NULL;
	}

	// Create a source playing it
	audio::Source* newSource = new audio::Source( newSound );
	newSource->options.looped = false;
	newSource->Play();
	return newSource;
}

audio::Source* engine::AudioInterface::LoopMusicFile ( const char* filename )
{
	// Create the sound
	audio::Buffer* newSound = audio::BufferManager::Active()->GetSound( filename );
	if ( !newSound ) {
		return NULL;
	}

	// Create the source playing it
	audio::Source* newSource = new audio::Source( newSound );
	newSource->options.looped = true;
	newSource->Play();
	return newSource;
}

//	PlaySound( soundscript ) : Creates a sound behavior from given soundscript.
// Sound behaviors are engine wrappers for audio::Source.
engine::Sound* engine::AudioInterface::PlaySound ( const char* soundscriptName )
{
	// Build file index map
	if ( scriptFileIndex.size() <= 0 ) {
		BuildIndexMap();
	} // End building index map

	arstring128 ar_soundName ( soundscriptName );

	// Find entry
	auto findResult = scriptFileIndex.find( ar_soundName );

	if ( findResult == scriptFileIndex.end() )
	{
		debug::Console->PrintWarning("Could not find sound entry `%s`\n", soundscriptName);
	}
	else
	{
		auto scriptResult = scriptList.find( ar_soundName );
		if ( scriptResult == scriptList.end() )
		{
			LoadEntry( soundscriptName, findResult->second );
		}

		scriptResult = scriptList.find( ar_soundName );
		//cout << "Found entry." << endl;

		// print entry properties
		/*cout << " AI:      " << scriptResult->second.ai_alert_amount << endl;
		cout << " pitch:   " << scriptResult->second.pitch << endl;
		cout << " atten:   " << scriptResult->second.attenuation << endl;
		cout << " Channel: ";
		switch ( scriptResult->second.channel ) {
			case CHAN_DEFAULT: cout << "CHAN_DEFAULT"; break;
			case CHAN_BACKGROUND: cout << "CHAN_BACKGROUND"; break;
			case CHAN_HEAVY: cout << "CHAN_HEAVY"; break;
			case CHAN_MUSIC: cout << "CHAN_MUSIC"; break;
			case CHAN_PHYSICS: cout << "CHAN_PHYSICS"; break;
			case CHAN_SPEECH: cout << "CHAN_SPEECH"; break;
		}
		cout << endl;
		cout << " wavecnt: " << (int)scriptResult->second.count << endl;
		for ( int i = 0; i < scriptResult->second.count; ++i ) {
			cout << "  wave " << i << ": " << scriptResult->second.sounds[i] << endl;
		}*/

		int chosen_sound_index = -1;

		// Do randomizer code to choose a sound to play
		if ( scriptResult->second.sound_count > 1 )
		{
			// Playing the sound
			int choice, current_index, chosen = 0, maxchoice;

			// Get max count of choices
			maxchoice = 0;
			for ( int i = 0; i < scriptResult->second.sound_count; ++i ) {
				if ( scriptResult->second.sound_usage[i] == 0 ) {
					maxchoice += 1;
				}
			}

			//cout << "Choice count " << maxchoice << endl;

			// Choose index
			if ( maxchoice > 1 ) {
				choice = (rand() % maxchoice)+1;
			}
			else {
				choice = 1;
			}

			// Convert choice to sound index
			current_index = 0;
			for ( int i = 0; i < scriptResult->second.sound_count; ++i ) {
				if ( scriptResult->second.sound_usage[i] == 0 ) {
					current_index += 1;
					if ( current_index == choice ) {
						chosen = i;
					}
				}
			}

			/*for ( int i = 0; i < scriptResult->second.count; ++i )
			{
				cout << "   usage[" << i << "]: " << (int)scriptResult->second.sound_usage[i] << endl;
			}
			cout << "  Play sound index: " << chosen << endl;*/

			// Set choice to used
			if ( maxchoice == 1 ) {
				// If only 1 choice, reset
				for ( int i = 0; i < scriptResult->second.sound_count; ++i ) {
					scriptResult->second.sound_usage[i] = 0;
				}
				scriptResult->second.sound_usage[chosen] = 2; // Set chosen to skip next run
			}
			else {
				// If only 1 choice, reset
				for ( int i = 0; i < scriptResult->second.sound_count; ++i ) {
					if ( scriptResult->second.sound_usage[i] == 2 ) { // Reset the skips
						scriptResult->second.sound_usage[i] = 0;
					}
				}
				scriptResult->second.sound_usage[chosen] = 1;
			}

			chosen_sound_index = chosen;
		}
		else {
			//cout << "  Play sound index: " << 0 << endl;
			chosen_sound_index = 0;
		}

		// Play sound
		if ( chosen_sound_index >= 0 )
		{
			// Create the sound
			string soundfilename =  core::Resources::PathTo( string("sounds/") + scriptResult->second.sounds[chosen_sound_index].c_str() );
			audio::Buffer* newSound = audio::BufferManager::Active()->GetSound( soundfilename.c_str() );

			// If sound driver is FUBAR, don't crash. Just warn about it.
			if ( newSound == NULL )
			{
				debug::Console->PrintWarning("Warning: sound engine could not load sound file! (is the device being whored?)\n");
			}
			
			// Create the source playing it
			audio::Source* newSource = new audio::Source( newSound );
			newSource->options.looped	= (scriptResult->second.loop != 0);
			newSource->options.pitch	= scriptResult->second.pitch;
			newSource->options.rolloff	= scriptResult->second.attenuation * 4.5f;
			newSource->options.gain		= scriptResult->second.gain;
			newSource->SetChannelProperties( scriptResult->second.channel );
			newSource->Play();

			// Create the behavior holding it
			engine::Sound* newSndef = new engine::Sound();
			newSndef->mySource			= newSource;
			newSndef->ai_alert_amount	= scriptResult->second.ai_alert_amount;
			newSndef->channel			= scriptResult->second.channel;
			newSndef->deleteWhenDone	= true;

			return newSndef;
		}
	}

	return NULL;
}


//	OpenSoundscriptListing( set ) : Opens a soundscript listing.
// Searches directories for a proper soundscript listing.
// Result should be closed with fclose().
FILE* engine::AudioInterface::OpenSoundscriptListing( audio::eSoundScriptSet set )
{
	FILE* fp = NULL;

	// Open proper file
	switch ( set )
	{
		case audio::kSetStandard:		fp = core::Resources::Open( "sounds/_def_standard.txt", "rb" ); break;
		case audio::kSetEnvironment:	fp = core::Resources::Open( "sounds/_def_environment.txt", "rb" ); break;
		case audio::kSetMusic:			fp = core::Resources::Open( "sounds/_def_music.txt", "rb" ); break;
		case audio::kSetVoice:			fp = core::Resources::Open( "sounds/_def_voice.txt", "rb" ); break;
		default:						fp = core::Resources::Open( "sounds/level_list.txt", "rb" ); break;
	}
	return fp;
}

void engine::AudioInterface::BuildIndexMap ( void )
{
	for ( uint8_t i = 0; i < audio::kSoundScriptSet_MAX; ++i )
	{
		// Load the default sound entry files
		FILE* fp = NULL;
		int read_mode = 0, prev_read_mode;
		string s_cur_read = "";
		int next_char;
		char buffer[256];
		int64_t buffer_pos;
		int64_t buffer_size;

		// Open proper file
		fp = OpenSoundscriptListing( (audio::eSoundScriptSet)i );

		if ( fp )
		{
			// Loop through the file, creating the entries
			while ( !feof( fp ) || ( read_mode == 2 ) || ( read_mode == 1 ) ) {
				// Looking for title
				if ( read_mode == 0 ) { 
					next_char = fgetc( fp );
					if ( next_char == '#' ) {
						prev_read_mode = read_mode;
						read_mode = 3;
					}
					else if ( isspace(next_char) )
					{
						// Check if valid name when hit the whitespace
						if ( s_cur_read.length() > 1 ) {
							// If valid name, look for first paren
							read_mode = 1;
							buffer_pos = 256;
						}
						else { // If invalid name, reset
							s_cur_read = "";
						}
					}
					else
					{
						// Add character to string if not a space or comment
						s_cur_read += (char)next_char;
					}
				}
				// Looking for first paren
				else if ( read_mode == 1 ) {
					// When find fist paren, save position and add new entry
					
					// Read in a block of data
					if ( buffer_pos >= 256 ) {
						buffer_pos = 0;
						buffer_size = fread( buffer, sizeof(char), 256, fp );
					}
					// Search the block of data
					while ( ( buffer_pos < buffer_size )&&( read_mode == 1 ) ) {
						if ( buffer[buffer_pos] != '{' ) {
							buffer_pos += 1;
						}
						else {
							// Save position and set in map
							audio::arSoundIndex index;
							index.pos = (uint32_t)(ftell( fp ) - buffer_size + buffer_pos);
							index.set = (audio::eSoundScriptSet)i;
							scriptFileIndex[arstring128(s_cur_read.c_str())] = index;

							// Go to next read mode
							read_mode = 2;
						}
					}
				}
				// Looking for end paren
				else if ( read_mode == 2 ) {
					// Read in a block of data
					if ( buffer_pos >= 256 ) {
						buffer_pos = 0;
						buffer_size = fread( buffer, sizeof(char), 256, fp );
					}
					while ( ( buffer_pos < buffer_size )&&( read_mode == 2 ) ) {
						if ( buffer[buffer_pos] != '}' ) {
							buffer_pos += 1;
						}
						else {
							// Go back to name reading mode
							read_mode = 0;
							s_cur_read = "";
							fseek( fp, (long)(-(buffer_size-1) + buffer_pos), SEEK_CUR ); // Go back to after the paren
						}
					}
				}
				// look for newline
				else if ( read_mode == 3 ) { 
					next_char = fgetc( fp );
					if ( next_char == '\n' ) {
						read_mode = prev_read_mode;
					}
				}
			} // End while loop
			fclose( fp );
		}
		else
		{
			debug::Console->PrintError("Could not open soundscript listing %d.\n", i);
		}
	}
}
void engine::AudioInterface::LoadEntry ( const char* soundName, const audio::arSoundIndex& indexInfo )
{
	debug::Console->PrintMessage("Caching sound entry \"%s\"\n", soundName);

	FILE* fp = NULL;
	audio::arSoundScript newScript = {0};

	// Set default options
	newScript.pitch			= 1.0F;
	newScript.sound_count	= 0;
	newScript.loop			= 0;
	newScript.gain			= 1.0F;
	newScript.attenuation	= 0.8F;

	switch ( indexInfo.set )
	{
	case audio::kSetStandard:
	default:
		newScript.channel			= audio::kChannelDefault;
		newScript.ai_alert_amount	= audio::kAIAlertSmall;
		newScript.attenuation		= 0.8f;
		break;
	case audio::kSetEnvironment:
		newScript.channel			= audio::kChannelBackground;
		newScript.ai_alert_amount	= audio::kAIAlertIgnore;
		newScript.attenuation		= 1.0f;
		break;
	case audio::kSetMusic:
		newScript.channel			= audio::kChannelMusic;
		newScript.ai_alert_amount	= audio::kAIAlertIgnore;
		newScript.attenuation		= 0.0f;
		break;
	case audio::kSetVoice:
		newScript.channel			= audio::kChannelSpeech;
		newScript.ai_alert_amount	= audio::kAIAlertIgnore;
		newScript.attenuation		= 0.0f;
		break;
	}
	
	// Open proper file
	fp = OpenSoundscriptListing( indexInfo.set );
	if (!fp)
	{
		debug::Console->PrintError("Could not open soundscript listing for set %d!\n", indexInfo.set);
	}

	// Go to pos in file
	fseek( fp, indexInfo.pos + 1, SEEK_SET );

	// Read all the key-value pairs
	bool can_continue = true;
	string s_cur_read = "";
	string s_cur_key;
	int next_char;
	int read_mode = 0;
	int prev_read_mode;
	while ( can_continue )
	{
		next_char = fgetc( fp );
		// look for key
		if ( read_mode == 0 ) { 
			// Check for comments
			if ( isspace(next_char) || ( next_char == '#' ) || ( next_char == '}' ) )
			{
				if ( next_char == '#' ) {
					prev_read_mode = read_mode;
					read_mode = 3;
				}
				else if ( next_char == '}' ) {
					can_continue = false;
				}
				else {
					// Check if valid name when hit the whitespace
					if ( s_cur_read.length() > 0 ) {
						// If valid name, found key, read value
						read_mode = 1;
						printf("key: `%s` ", s_cur_read.c_str());
						s_cur_key = s_cur_read;
						s_cur_read = "";
					}
					else { // If invalid name, reset
						s_cur_read = "";
					}
				}
			}
			// Otherwise add to string
			else {
				s_cur_read += (char)next_char;
			}
		}
		else if ( (read_mode == 1) || (read_mode == 2) ) {
			// Check for comments
			if ( read_mode == 1 )
			{
				if ( isspace(next_char) || ( next_char == '#' ) )
				{
					if ( next_char == '#' ) {
						prev_read_mode = read_mode;
						read_mode = 3;
					}
					else {
						// Check if valid name when hit the whitespace
						if ( s_cur_read.length() > 0 ) {
							// If valid name, found value, go back to reading key
							read_mode = 0;
							printf("value: `%s` \n", s_cur_read.c_str());
							// Edit soundscript
							EditSoundScript( newScript, s_cur_key, s_cur_read );
							// Reset read
							s_cur_read = "";
						}
						else { // If invalid name, reset
							s_cur_read = "";
						}
					}
				}
				// Otherwise add to string
				else {
					if ( next_char == '"' ) {
						s_cur_read = "";
						read_mode = 2;
					}
					else {
						s_cur_read += (char)next_char;
					}
				}
			}
			else if ( read_mode == 2 )
			{
				// Read in all characters, including spaces
				if ( next_char == '"' ) {
					read_mode = 1;
				}
				else {
					s_cur_read += (char)next_char;
				}
			}
		}
		// look for newline
		else if ( read_mode == 3 ) { 
			if ( next_char == '\n' ) {
				read_mode = prev_read_mode;
			}
		}
	} // End loop

	fclose( fp );

	// Add to map
	scriptList[arstring<128>(soundName)] = newScript;
}

void engine::AudioInterface::EditSoundScript ( audio::arSoundScript& script, const string& key, const string& value )
{
	if ( key == "channel" ) {
		if ( value == "voice" || value == "speech" ) {
			script.channel = audio::kChannelSpeech;
			script.ai_alert_amount = audio::kAIAlertSpeech;
		}
		else if ( value == "physics" ) {
			script.channel = audio::kChannelPhysics;
			script.ai_alert_amount = audio::kAIAlertSmall;
		}
		else if ( value == "heavy" ) {
			script.channel = audio::kChannelHeavy;
			script.ai_alert_amount = audio::kAIAlertNotice;
		}
		else if ( value == "default" ) {
			script.channel = audio::kChannelDefault;
			script.ai_alert_amount = audio::kAIAlertSmall;
		}
		else if ( value == "background" ) {
			script.channel = audio::kChannelBackground;
			script.ai_alert_amount = audio::kAIAlertIgnore;
		}
		else if ( value == "music" ) {
			script.channel = audio::kChannelMusic;
			script.ai_alert_amount = audio::kAIAlertIgnore;
		}
	}
	else if ( key == "ai" ) {
		if ( value == "3" ) {
			script.ai_alert_amount = audio::kAIAlertNotice;
		}
		else if ( value == "2" ) {
			script.ai_alert_amount = audio::kAIAlertSpeech;
		}
		else if ( value == "1" ) {
			script.ai_alert_amount = audio::kAIAlertSmall;
		}
		else if ( value == "0" ) {
			script.ai_alert_amount = audio::kAIAlertIgnore;
		}
	}
	else if ( key == "volume" ) {
		if ( value == "0" ) {
			script.attenuation = 0;
		}
	}
	else if ( key == "wave" ) {
		// Push back onto wave list
		if ( script.sound_count < 4 ) {
			strcpy( script.sounds[script.sound_count], value.c_str() );
			script.sound_count += 1;
		}
	}
	else if ( key == "loop" ) {
		if ( value == "1" ) {
			script.loop = 1;
		}
	}
	else if ( key == "pitch" ) {
		script.pitch = (float)atof( value.c_str() );
	}
	else if ( key == "gain" ) {
		script.gain = (float)atof( value.c_str() );
	}
	else if ( key == "attenuation" ) {
		script.attenuation = (float)atof( value.c_str() );
	}
}