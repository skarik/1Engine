
#include "core-ext/system/io/Resources.h"
#include "CAudioInterface.h"
#include "audio/CAudioMaster.h"
#include "audio/CAudioListener.h"
#include "audio/CAudioSource.h"
#include "audio/CAudioSound.h"
#include "audio/CSoundManager.h"

using namespace AudioStructs;
using namespace std;

CAudioInterface Audio;

vector<CAudioSource*> CAudioInterface::GetCurrentSources ( void )
{
	return *CAudioMaster::GetCurrent()->GetSources();
}

CAudioListener* CAudioInterface::CreateListener ( void )
{
	return new CAudioListener();
}

CAudioSource* CAudioInterface::PlayWaveFile ( const char* wavname )
{
	// First create the sound
	CAudioSound* newSound = CSoundManager::GetActive()->GetSound( wavname );
	if ( !newSound ) {
		return NULL;
	}

	// Create a source playing it
	CAudioSource* newSource = new CAudioSource( newSound );
	newSource->options.looped = false;
	newSource->Play();
	return newSource;
}

CAudioSource* CAudioInterface::LoopMusicFile ( const char* fukebane )
{
	// Create the sound
	CAudioSound* newSound = CSoundManager::GetActive()->GetSound( fukebane );
	if ( !newSound ) {
		return NULL;
	}

	// Create the source playing it
	CAudioSource* newSource = new CAudioSource( newSound );
	newSource->options.looped = true;
	newSource->Play();
	return newSource;
}

#include <stdio.h>
#include <ctype.h>
//#include "CSoundBehavior.h"

CSoundBehavior* CAudioInterface::playSound ( const char* soundName )
{
	// Build file index map
	if ( scriptFileIndex.size() <= 0 ) {
		BuildIndexMap();
	} // End building index map

	arstring<128> ar_soundName ( soundName );

	// Find entry
	unordered_map<arstring<128>, soundIndex_t>::iterator findResult = scriptFileIndex.find( ar_soundName );

	if ( findResult == scriptFileIndex.end() ) { 
		cout << "Could not find sound entry \"" << soundName << "\"" << endl;
	}
	else {
		//cout << "Found sound entry \"" << soundName << "\"" << endl;

		unordered_map<arstring<128>, soundScript_t>::iterator scriptResult = scriptList.find( ar_soundName );
		if ( scriptResult == scriptList.end() ) {
			LoadEntry( soundName, findResult->second );
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
		if ( scriptResult->second.count > 1 )
		{
			// Playing the sound
			int choice, current_index, chosen, maxchoice;

			// Get max count of choices
			maxchoice = 0;
			for ( int i = 0; i < scriptResult->second.count; ++i ) {
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
			for ( int i = 0; i < scriptResult->second.count; ++i ) {
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
				for ( int i = 0; i < scriptResult->second.count; ++i ) {
					scriptResult->second.sound_usage[i] = 0;
				}
				scriptResult->second.sound_usage[chosen] = 2; // Set chosen to skip next run
			}
			else {
				// If only 1 choice, reset
				for ( int i = 0; i < scriptResult->second.count; ++i ) {
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
			CAudioSound* newSound = CSoundManager::GetActive()->GetSound( soundfilename.c_str() );

			// If sound driver is FUBAR, don't crash. Just warn about it.
			if ( newSound == NULL )
			{
				cout << "Warning: sound engine could not load sound file! (is the device being whored?)" << endl;
			}
			
			// Create the source playing it
			CAudioSource* newSource = new CAudioSource( newSound );
			newSource->options.looped	= (scriptResult->second.loop != 0);
			newSource->options.pitch	= scriptResult->second.pitch;
			newSource->options.rolloff	= scriptResult->second.attenuation * 4.5f;
			newSource->options.gain		= scriptResult->second.gain;
			newSource->SetChannelProperties( scriptResult->second.channel );
			newSource->Play();

			// Create the behavior holding it
			CSoundBehavior* newSndef = new CSoundBehavior();
			newSndef->mySource = newSource;
			newSndef->ai_alert_amount	= scriptResult->second.ai_alert_amount;
			newSndef->channel			= scriptResult->second.channel;
			newSndef->deleteWhenDone	= true;
			newSndef->filename			= soundfilename;

			return newSndef;
		}
	}

	return NULL;
}


void CAudioInterface::BuildIndexMap ( void )
{
	for ( char i = 0; i < 4; ++i )
	{
		// Load the default sound entry files
		FILE* fp = NULL;
		int read_mode = 0, prev_read_mode;
		string s_cur_read = "";
		int next_char;
		char buffer[256];
		int buffer_pos;
		int buffer_size;

		// Open proper file
		switch ( i )
		{
			case 0:		fp = core::Resources::Open( "sounds/_def_standard.txt", "rb" ); break;
			case 1:		fp = core::Resources::Open( "sounds/_def_environment.txt", "rb" ); break;
			case 2:		fp = core::Resources::Open( "sounds/_def_music.txt", "rb" ); break;
			case 3:		fp = core::Resources::Open( "sounds/_def_voice.txt", "rb" ); break;
			default:	fp = core::Resources::Open( "sounds/level_list.txt", "rb" ); break;
		}

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
							soundIndex_t index;
							index.pos = ftell( fp ) - buffer_size + buffer_pos;
							index.set = i;
							scriptFileIndex[arstring<128>(s_cur_read.c_str())] = index;

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
							fseek( fp, -(buffer_size-1) + buffer_pos, SEEK_CUR ); // Go back to after the paren
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
			throw core::NullReferenceException();
		}
	}
}
void CAudioInterface::LoadEntry ( const char* soundName, const soundIndex_t& indexInfo )
{
	cout << "Could not find entry saved, caching." << endl;

	soundScript_t newScript;
	FILE* fp;

	// Set default options
	newScript.pitch = 1.0f;
	newScript.count = 0;
	newScript.loop = 0;
	newScript.gain = 1.0f;
	newScript.attenuation = 0.8F;
	memset( newScript.sound_usage, 0, 4 );
	memset( newScript.sounds, 0, 256*4 );
	switch ( indexInfo.set )
	{
	case 0:
	default:
		newScript.channel			= CHAN_DEFAULT;
		newScript.ai_alert_amount	= AI_SMALL;
		newScript.attenuation		= 0.8f;
		break;
	case 1:
		newScript.channel			= CHAN_BACKGROUND;
		newScript.ai_alert_amount	= AI_IGNORE;
		newScript.attenuation		= 1.0f;
		break;
	case 2:
		newScript.channel			= CHAN_MUSIC;
		newScript.ai_alert_amount	= AI_IGNORE;
		newScript.attenuation		= 0.0f;
		break;
	case 3:
		newScript.channel			= CHAN_SPEECH;
		newScript.ai_alert_amount	= AI_IGNORE;
		newScript.attenuation		= 0.0f;
		break;
	}
	
	// Open proper file
	switch ( indexInfo.set )
	{
		case 0:		fp = core::Resources::Open( "sounds/_def_standard.txt", "rb" ); break;
		case 1:		fp = core::Resources::Open( "sounds/_def_environment.txt", "rb" ); break;
		case 2:		fp = core::Resources::Open( "sounds/_def_music.txt", "rb" ); break;
		case 3:		fp = core::Resources::Open( "sounds/_def_voice.txt", "rb" ); break;
		default:	fp = core::Resources::Open( "sounds/level_list.txt", "rb" ); break;
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
						cout << "key: -" << s_cur_read << "- ";
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
							cout << "value: -" << s_cur_read << "- " << endl;
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

void CAudioInterface::EditSoundScript ( soundScript_t & script, const string& key, const string& value )
{
	if ( key == "channel" ) {
		if ( value == "voice" || value == "speech" ) {
			script.channel = CHAN_SPEECH;
			script.ai_alert_amount = AI_SPEECH;
		}
		else if ( value == "physics" ) {
			script.channel = CHAN_PHYSICS;
			script.ai_alert_amount = AI_SMALL;
		}
		else if ( value == "heavy" ) {
			script.channel = CHAN_HEAVY;
			script.ai_alert_amount = AI_NOTICE;
		}
		else if ( value == "default" ) {
			script.channel = CHAN_DEFAULT;
			script.ai_alert_amount = AI_SMALL;
		}
		else if ( value == "background" ) {
			script.channel = CHAN_BACKGROUND;
			script.ai_alert_amount = AI_IGNORE;
		}
		else if ( value == "music" ) {
			script.channel = CHAN_MUSIC;
			script.ai_alert_amount = AI_IGNORE;
		}
	}
	else if ( key == "ai" ) {
		if ( value == "3" ) {
			script.ai_alert_amount = AI_NOTICE;
		}
		else if ( value == "2" ) {
			script.ai_alert_amount = AI_SPEECH;
		}
		else if ( value == "1" ) {
			script.ai_alert_amount = AI_SMALL;
		}
		else if ( value == "0" ) {
			script.ai_alert_amount = AI_IGNORE;
		}
	}
	else if ( key == "volume" ) {
		if ( value == "0" ) {
			script.attenuation = 0;
		}
	}
	else if ( key == "wave" ) {
		// Push back onto wave list
		if ( script.count < 4 ) {
			strcpy( script.sounds[script.count], value.c_str() );
			script.count += 1;
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