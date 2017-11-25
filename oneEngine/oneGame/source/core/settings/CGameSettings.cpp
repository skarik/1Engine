#include "CGameSettings.h"
#include "core/containers/arstring.h"
#include "core/utils/StringUtils.h"
#include "core/exceptions/exceptions.h"
#include "core-ext/system/io/mccosf.h"
#include "core-ext/system/io/Resources.h"

#include "core/system/io/FileUtils.h"

#include <fstream>
using std::fstream;
using std::ifstream;
using std::ofstream;
#include <iostream>
#include <sstream>
using std::stringstream;

#include <cctype>

// Static Variable Declarations
CGameSettings* CGameSettings::pActive = NULL;

// == Static Functions== 
// Get Active Settings
CGameSettings* CGameSettings::Active ( void )
{
	return pActive;
}
// Set Active Settings
void CGameSettings::SetActive ( CGameSettings* instance )
{
	pActive = instance;
}

// == Constructor ==
CGameSettings::CGameSettings ( string& command_line )
{
	pActive = this;
	Initialize();

	s_cmd = command_line;

	bool in_arg = false;
	bool in_quote = false;
	int i_start;
	int i = 0;
	while ( i < (int)command_line.length() )
	{
		// Looking for arg:
		if ( !in_arg )
		{
			if ( !std::isspace(command_line[i]) )
			{
				i_start = i;
				in_arg = true;

				if ( command_line[i] == '"' )
				{
					in_quote = true;
				}
			}

			i += 1;
		}
		// Found arg:
		else
		{
			if ( !in_quote && std::isspace(command_line[i]) )
			{
				in_arg = false;
				m_cmd.push_back(command_line.substr(i_start, i - i_start));
			}
			else if ( in_quote && command_line[i] == '"' )
			{
				in_quote = false;
			}
			else if ( !in_quote && command_line[i] == '"' )
			{
				in_quote = true;
			}

			i += 1;
		}
	}
	// Add the last one:
	if (in_arg)
	{
		m_cmd.push_back(command_line.substr(i_start, i - i_start));
	}
	// Strip quotes on settings:
	for (size_t str_i = 0; str_i < m_cmd.size(); ++str_i)
	{
		if (std::count(m_cmd[str_i].begin(), m_cmd[str_i].end(), '"') == 2)
		{
			if (m_cmd[str_i][0] == '"' && m_cmd[str_i][m_cmd[str_i].length() - 1] == '"')
			{
				m_cmd[str_i] = m_cmd[str_i].substr(1, m_cmd[str_i].length() - 2);
			}
		}
	}
}

// == Constructor ==
CGameSettings::CGameSettings ( char** command_line )
{
	pActive = this;
	Initialize();

	// Add command line to the list:
	s_cmd = command_line[0];
	int str_i = 0;
	char* str = command_line[str_i];
	while (str != NULL)
	{
		m_cmd.push_back(str);
		++str_i;
		str = command_line[str_i];
	}
	// Strip quotes on settings:
	for (size_t str_i = 0; str_i < m_cmd.size(); ++str_i)
	{
		if (std::count(m_cmd[str_i].begin(), m_cmd[str_i].end(), '"') == 2)
		{
			if (m_cmd[str_i][0] == '"' && m_cmd[str_i][m_cmd[str_i].length() - 1] == '"')
			{
				m_cmd[str_i] = m_cmd[str_i].substr(1, m_cmd[str_i].length() - 2);
			}
		}
	}
}

void CGameSettings::Initialize ( void )
{
#	ifdef _WIN32
	// Fix the working directory:
	if (!IO::FileExists("./oneEngine.dll") && !IO::FileExists("./.res-0/system/properties.txt"))
	{
		char path [512];
		GetModuleFileName(NULL, path, sizeof(path));
		for (int i = sizeof(path)-1; i > 0; --i)
		{
			if (path[i] == '\\' || path[i] == '/')
			{
				path[i] = 0;
				break;
			}
		}
		SetCurrentDirectory(path);
	}
#	endif

	m_target_file_world		= "terra";
	m_target_file_realm		= "_lucra";
	m_target_file_player	= "_default";

	MakeDirectory( ".game" );
	MakeDirectory( ".game/.realms" );
	MakeDirectory( ".game/.players" );
	MakeDirectory( ".game/.dfshaders" );

	// Set up external setting variables
	LinkSettingVariable( b_dbg_logToFile, "dbg_logtofile" );

	LinkSettingVariable( b_ro_EnableShaders,	"ro_enableshaders" );
	LinkSettingVariable( i_ro_TargetResX,	"ro_resolutionx" );
	LinkSettingVariable( i_ro_TargetResY,	"ro_resolutiony" );
	LinkSettingVariable( i_ro_ShadowMapResolution,	"ro_shadowresolution" );
	LinkSettingVariable( b_ro_EnableShadows,	"ro_enableshadows" );
	LinkSettingVariable( i_ro_GrassSubdivisions,	"ro_grasssubdivisions" );
	LinkSettingVariable( b_ro_UseBufferModel,	"ro_usebuffermodel" );
	LinkSettingVariable( b_ro_UseHighRange,	"ro_usehighrange" );
	LinkSettingVariable( i_ro_RendererMode,	"ro_renderer_mode" );
	LinkSettingVariable( b_ro_Enable30Blit,	"ro_enable30blit" );
	LinkSettingVariable( b_ro_Enable30Steroscopic,	"ro_enable30steroscopic" );
	LinkSettingVariable( f_ro_DefaultCameraRange,	"ro_camerarangedefault" );

	LinkSettingVariable( b_cl_FastStart,	"cl_faststart" );
	LinkSettingVariable( i_cl_DefaultSeed,	"cl_defaultseed" );
	LinkSettingVariable( b_cl_MinimizeTerrainThreads,	"cl_minimal_terrain_threads" );
	LinkSettingVariable( i_cl_KeyboardStyle,	"cl_keyboardstyle" );
	LinkSettingVariable( f_cl_MouseSensitivity,	"cl_mouse_sensitivity" );
	LinkSettingVariable( f_cl_ReticleScale,	"cl_reticlescale" );

	LinkSettingVariable( i_cl_ter_Range,	"cl_ter_range" );
	LinkSettingVariable( b_cl_ter_ShadowOfTheCollussusRenderStyle,	"cl_ter_shadowofthecollussusrenderstyle" );
	LinkSettingVariable( i_cl_ter_FarLODRangeCount,	"cl_ter_farlodrangecount" );

	LinkSettingVariable( b_dbg_ro_ShowMissingLinks,	"dbg_ro_showmissinglinks" );

	// Load all the variables
	LoadSettings ();

	// Set MISC non-external settings
	b_dbg_ro_EnableSkinning = 1;
	b_dbg_ro_GPUSkinning = 0;
}

// == Load Settings ==
void CGameSettings::LoadSettings ( void )
{
	/*
	Console->Link( "ro_enableshaders", &b_ro_EnableShaders );
	Console->Link( "ro_resolutionx", &i_ro_TargetResX );
	Console->Link( "ro_resolutiony", &i_ro_TargetResY );
	Console->Link( "ro_shadowresolution", &i_ro_ShadowMapResolution );

	Console->RunExternal( ".game\\gameoptions.cfg" );
	*/

	// Set default options
	b_dbg_logToFile = false;

	b_ro_EnableShaders = true;
	
	i_ro_TargetResX = 1280;
	i_ro_TargetResY = 720;

	i_ro_ShadowMapResolution = 512;
	b_ro_EnableShadows = true;

	i_ro_GrassSubdivisions = 2;

	b_ro_UseBufferModel = true;
	b_ro_UseHighRange = true;
	i_ro_RendererMode = 0x2; // Deferred rendering
	b_ro_Enable30Blit = true;
	b_ro_Enable30Steroscopic = false;
	f_ro_DefaultCameraRange = 1000;

	b_cl_FastStart = false;
	i_cl_DefaultSeed = 0;
	b_cl_MinimizeTerrainThreads = false;

	i_cl_KeyboardStyle = 0;
	f_cl_MouseSensitivity = 0.6f;

	f_cl_ReticleScale = 1.0f;
	
	i_cl_ter_Range = 5;
	b_cl_ter_ShadowOfTheCollussusRenderStyle = true;
	i_cl_ter_FarLODRangeCount = 1;

	b_dbg_ro_ShowMissingLinks = 0;

	// read in file line by line
	ifstream infile ( ".game/gameoptions.cfg" );
	
	if ( infile.is_open() )
	{
		char cmd [256];
		while ( !infile.eof() )
		{
			infile.getline( cmd, 256 );
			stringstream ss;
			ss.str( string( cmd ) );

			string sCommand;
			ss >> sCommand;
			sCommand = StringUtils::ToLower( sCommand );

			// Is it a valid command string?
			if ( sCommand.length() > 1 )
			{
				// TODO: Run the command in the console. Certain command may be able to be run at this point.

				// Search the first two lists for the variable
				{
					auto t_cmd = m_settings_pint.find(sCommand);
					if ( t_cmd != m_settings_pint.end() )
					{	// Read in an int
						ss >> *(t_cmd->second);
						continue; // Goto next command
					}
				}
				{
					auto t_cmd = m_settings_preal.find(sCommand);
					if ( t_cmd != m_settings_preal.end() )
					{	// Read in a Real
						ss >> *(t_cmd->second);
						continue; // Goto next command
					}
				}

				// If it cannot be found, read the value into a Real (unless it's a string, but we don't support that right now).
				{
					Real t_value;
					ss >> t_value;
					m_settings_real[sCommand] = t_value;
					continue; // Goto next command
				}
			}
		} // End reading loop
	}

	b_ro_UseBufferModel = 1; // Override with buffer model rendering
	b_ro_UseHighRange = 1; // Override with HDR rendering
	i_ro_RendererMode = 0x2; // Override with deferred rendering

	b_cl_ter_ShadowOfTheCollussusRenderStyle = 1; // Override and enable the SOTC-style LOD system

	// save current settings
	SaveSettings();


	// Load up the properties file
	sysprop_developerstring	= "Development Build 05";
	sysprop_default_cmd		= "";

	FILE* fp_file = core::Resources::Open("system/properties.txt","rb");
	if ( fp_file != NULL )
	{
		COSF_Loader loader ( fp_file );
		mccOSF_entry_info_t entry;
		
		// Loop through all the entries
		do
		{
			// Get next entry
			loader.GetNext( entry );
			if ( entry.type == MCCOSF_ENTRY_NORMAL || entry.type == MCCOSF_ENTRY_END )
			{
				// Check the key
				if ( strcmp( entry.name, "developerstring" ) == 0 )
				{
					sysprop_developerstring = entry.value;
				}
				else if ( strcmp( entry.name, "defaultcmd" ) == 0 )
				{
					sysprop_default_cmd = entry.value;
				}
				else if ( strcmp( entry.name, "editor" ) == 0 )
				{
					sysprop_editor = entry.value;
				}
			}
		}
		while ( entry.type != MCCOSF_ENTRY_EOF );

		// Close the file
		fclose( fp_file );
	}
}

// == Save Settings ==
void CGameSettings::SaveSettings ( void )
{
	// output file
	ofstream outfile ( ".game/gameoptions.cfg" );

	/*outfile << "ro_enableshaders " << b_ro_EnableShaders << std::endl;
	outfile << "ro_resolutionx " << i_ro_TargetResX << std::endl;
	outfile << "ro_resolutiony " << i_ro_TargetResY << std::endl;
	outfile << "ro_enableshadows " << b_ro_EnableShadows << std::endl;
	outfile << "ro_shadowresolution " << i_ro_ShadowMapResolution << std::endl;
	outfile << "ro_grasssubdivisions " << i_ro_GrassSubdivisions << std::endl;
	outfile << std::endl;
	outfile << "ro_usebuffermodel " << b_ro_UseBufferModel << std::endl;
	outfile << "ro_usehighrange " << b_ro_UseHighRange << std::endl;
	outfile << "ro_renderer_mode " << i_ro_RendererMode << std::endl;
	outfile << "ro_Enable30Blit " << b_ro_Enable30Blit << std::endl;
	outfile << "ro_Enable30Steroscopic " << b_ro_Enable30Steroscopic << std::endl;
	outfile << "ro_CameraRangeDefault " << f_ro_DefaultCameraRange << std::endl;
	outfile << std::endl;
	outfile << "cl_faststart " << b_cl_FastStart << std::endl;
	outfile << "cl_defaultseed " << i_cl_DefaultSeed << std::endl;
	outfile << "cl_minimal_terrain_threads " << b_cl_MinimizeTerrainThreads << std::endl;
	outfile << std::endl;
	outfile << "cl_keyboardstyle " << i_cl_KeyboardStyle << std::endl;
	outfile << "cl_mouse_sensitivity " << f_cl_MouseSensitivity << std::endl;
	outfile << std::endl;
	outfile << "cl_reticlescale " << f_cl_ReticleScale << std::endl;
	outfile << std::endl;
	outfile << "cl_ter_range " << i_cl_ter_Range << std::endl;
	outfile << "cl_ter_shadowofthecollussusrenderstyle " << b_cl_ter_ShadowOfTheCollussusRenderStyle << std::endl;
	outfile << "cl_ter_farlodrangecount " << i_cl_ter_FarLODRangeCount << std::endl;
	outfile << std::endl;
	outfile << "dbg_ro_ShowMissingLinks " << b_dbg_ro_ShowMissingLinks << std::endl;
	outfile << std::endl;
	outfile << std::endl;*/

	/*EditSaveSetting( "ro_enableshaders", b_ro_EnableShaders );
	EditSaveSetting( "ro_resolutionx", i_ro_TargetResX );
	EditSaveSetting( "ro_resolutiony", i_ro_TargetResY );
	EditSaveSetting( "ro_enableshadows", b_ro_EnableShadows );
	EditSaveSetting( "ro_shadowresolution", i_ro_ShadowMapResolution );
	EditSaveSetting( "ro_grasssubdivisions", i_ro_GrassSubdivisions );

	EditSaveSetting( "ro_usebuffermodel", b_ro_UseBufferModel );
	EditSaveSetting( "ro_usehighrange", b_ro_UseHighRange );
	EditSaveSetting( "ro_renderer_mode", i_ro_RendererMode );
	EditSaveSetting( "ro_Enable30Blit", b_ro_Enable30Blit );
	EditSaveSetting( "ro_Enable30Steroscopic", b_ro_Enable30Steroscopic );
	EditSaveSetting( "ro_CameraRangeDefault", f_ro_DefaultCameraRange );

	EditSaveSetting( "cl_faststart", b_cl_FastStart );
	EditSaveSetting( "cl_defaultseed", i_cl_DefaultSeed );
	EditSaveSetting( "cl_minimal_terrain_threads", b_cl_MinimizeTerrainThreads );

	EditSaveSetting( "cl_keyboardstyle", i_cl_KeyboardStyle );
	EditSaveSetting( "cl_mouse_sensitivity", f_cl_MouseSensitivity );

	EditSaveSetting( "cl_reticlescale", f_cl_ReticleScale );

	EditSaveSetting( "cl_ter_range", i_cl_ter_Range );
	EditSaveSetting( "cl_ter_shadowofthecollussusrenderstyle", b_cl_ter_ShadowOfTheCollussusRenderStyle );
	EditSaveSetting( "cl_ter_farlodrangecount", i_cl_ter_FarLODRangeCount );

	EditSaveSetting( "dbg_ro_ShowMissingLinks", b_dbg_ro_ShowMissingLinks );*/

	// Output all values to the file
	for ( auto t_value = m_settings_pint.begin(); t_value != m_settings_pint.end(); t_value++ )
		outfile << t_value->first << " " << *t_value->second << std::endl;
	for ( auto t_value = m_settings_preal.begin(); t_value != m_settings_preal.end(); t_value++ )
		outfile << t_value->first << " " << *t_value->second << std::endl;

	// TODO: Check the following lists for duplicates first
	for ( auto t_value = m_settings_int.begin(); t_value != m_settings_int.end(); t_value++ )
		outfile << t_value->first << " " << t_value->second << std::endl;
	for ( auto t_value = m_settings_real.begin(); t_value != m_settings_real.end(); t_value++ )
		outfile << t_value->first << " " << t_value->second << std::endl;
	for ( auto t_value = m_settings_string.begin(); t_value != m_settings_string.end(); t_value++ )
		outfile << t_value->first << " " << t_value->second << std::endl;

	outfile << std::endl;
}

//template <typename Type>
//void CGameSettings::EditSaveSetting ( const char * s_id, const Type value )
//{
	/*FILE* fp_gameSettings = fopen( ".game/gameoptions.cfg", "+rb" );

	arstring<512> inputLine;

	while ( !feof(fp_gameSettings) )
	{
		
	}*/
	/*ifstream fs_gameSettings;
	ofstream fo_gameSettings;
	fs_gameSettings.open( ".game/gameoptions.cfg" );
	fo_gameSettings.open( ".game/gameoptions.cfg.temp" );
	if ( fs_gameSettings.is_open() )
	{
		arstring<512> inputLine;
		string sCommand;
		string sComparative;
		size_t tPosition;

		// Get command to compare to
		sComparative = StringUtils::ToLower( s_id );

		while ( !fs_gameSettings.eof() )
		{
			fs_gameSettings.getline( inputLine, 256 );
			stringstream ss;
			ss.str( string( inputLine ) );

			ss >> sCommand;
			sCommand = StringUtils::ToLower( sCommand );
			if ( sCommand == sComparative )
			{
				// match, so output edited line
			}
			else
			{
				// Nothing changed, don't change anything
				fo_gameSettings << inputLine;
			}
		}
		fs_gameSettings.close();
	}*/
//}

// Get all target resolutions
/*
const vector<CGameSettings::resolution_t> & CGameSettings::GetResolutionList ( void )
{
	static vector<resolution_t> resolutions;

	if ( resolutions.empty() ) {
#ifdef _WIN32
		// Generate resolution list
		DEVMODE dm = { 0 };
		dm.dmSize = sizeof(dm);
		for( int iModeNum = 0; EnumDisplaySettings( NULL, iModeNum, &dm ) != 0; ++iModeNum )
		{
			resolution_t newres;
			newres.w = dm.dmPelsWidth;
			newres.h = dm.dmPelsHeight;

			if ( dm.dmPelsHeight >= 480 ) {
				vector<resolution_t>::iterator found_value = find( resolutions.begin(), resolutions.end(), newres );

				if ( found_value == resolutions.end() )
					resolutions.push_back( newres );
			}
		}
#endif
	}

	return resolutions;
}*/


//		LinkSettingVariable
// Adds a variable to the list to be updated from and to the current gameoptions.cfg file
void CGameSettings::LinkSettingVariable ( int& variable, const char * s_id )
{
	string t_actual_name = StringUtils::ToLower( s_id );
	if ( m_settings_pint.find(t_actual_name) == m_settings_pint.end() )
	{
		m_settings_pint[t_actual_name] = &variable;
	}
	else
	{
		throw core::InvalidCallException();
	}
}
void CGameSettings::LinkSettingVariable ( Real& variable, const char * s_id )
{
	string t_actual_name = StringUtils::ToLower( s_id );
	if ( m_settings_preal.find(t_actual_name) == m_settings_preal.end() )
	{
		m_settings_preal[t_actual_name] = &variable;
	}
	else
	{
		throw core::InvalidCallException();
	}
}

//		EditSetting
// Either adds a new setting or changes an existing setting in the corresponding list.
void CGameSettings::EditSetting ( const char * s_id, const int value )
{
	string t_actual_name = StringUtils::ToLower( s_id );
	// Remove the value from the other two lists
	auto t_cmd0 = m_settings_real.find( t_actual_name );
	if ( t_cmd0 != m_settings_real.end() )
		m_settings_real.erase( t_cmd0 );
	auto t_cmd1 = m_settings_string.find( t_actual_name );
	if ( t_cmd1 != m_settings_string.end() )
		m_settings_string.erase( t_cmd1 );
	// Set it in the current list
	m_settings_int[t_actual_name] = value;
}
void CGameSettings::EditSetting ( const char * s_id, const Real value )
{
	string t_actual_name = StringUtils::ToLower( s_id );
	// Remove the value from the other two lists
	auto t_cmd0 = m_settings_int.find( t_actual_name );
	if ( t_cmd0 != m_settings_int.end() )
		m_settings_int.erase( t_cmd0 );
	auto t_cmd1 = m_settings_string.find( t_actual_name );
	if ( t_cmd1 != m_settings_string.end() )
		m_settings_string.erase( t_cmd1 );
	// Set it in the current list
	m_settings_real[t_actual_name] = value;
}
void CGameSettings::EditSetting ( const char * s_id, const char* value )
{
	string t_actual_name = StringUtils::ToLower( s_id );
	// Remove the value from the other two lists
	auto t_cmd0 = m_settings_int.find( t_actual_name );
	if ( t_cmd0 != m_settings_int.end() )
		m_settings_int.erase( t_cmd0 );
	auto t_cmd1 = m_settings_real.find( t_actual_name );
	if ( t_cmd1 != m_settings_real.end() )
		m_settings_real.erase( t_cmd1 );
	// Set it in the current list
	m_settings_string[t_actual_name] = value;
}

//		GetSettingExists
// Returns if the setting with the given name exists
bool CGameSettings::GetSettingExists ( const char * s_id )
{
	string t_actual_name = StringUtils::ToLower( s_id );
	// Search all lists for the value
	if ( m_settings_pint.find(t_actual_name) != m_settings_pint.end() ) return true;
	if ( m_settings_preal.find(t_actual_name) != m_settings_preal.end() ) return true;
	if ( m_settings_int.find(t_actual_name) != m_settings_int.end() ) return true;
	if ( m_settings_real.find(t_actual_name) != m_settings_real.end() ) return true;
	if ( m_settings_string.find(t_actual_name) != m_settings_string.end() ) return true;
	// Nothing? Then it's not here.
	return false;
}

//		GetSettingAs*
// Grabs a setting from the key-value list.
const int CGameSettings::GetSettingAsInt ( const char * s_id )
{
	string t_actual_name = StringUtils::ToLower( s_id );
#ifdef _ENGINE_DEBUG
	if ( t_actual_name.length() < 1 ) throw core::InvalidArgumentException();
#endif
	// Search the list of reals first.
	{
		auto t_cmd = m_settings_real.find( s_id );
		if ( t_cmd != m_settings_real.end() )
		{
			return (int)t_cmd->second;
		}
	}
	// Return 0 for invalids.
	return 0;
}
const Real CGameSettings::GetSettingAsReal ( const char * s_id )
{
	string t_actual_name = StringUtils::ToLower( s_id );
#ifdef _ENGINE_DEBUG
	if ( t_actual_name.length() < 1 ) throw core::InvalidArgumentException();
#endif
	// Search the list of reals first.
	{
		auto t_cmd = m_settings_real.find( s_id );
		if ( t_cmd != m_settings_real.end() )
		{
			return t_cmd->second;
		}
	}
	// Return 0 for invalids.
	return 0;
}
const string CGameSettings::GetSettingAsString ( const char * s_id )
{
	string t_actual_name = StringUtils::ToLower( s_id );
#ifdef _ENGINE_DEBUG
	if ( t_actual_name.length() < 1 ) throw core::InvalidArgumentException();
#endif
	// Search the list of strings first.
	{
		auto t_cmd = m_settings_string.find( s_id );
		if ( t_cmd != m_settings_string.end() )
		{
			return t_cmd->second;
		}
	}
	// Return empty for invalids.
	return string();
}