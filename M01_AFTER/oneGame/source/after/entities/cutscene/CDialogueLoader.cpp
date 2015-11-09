
#include "CDialogueLoader.h"
#include "core/debug/CDebugConsole.h"
#include "core-ext/system/io/Resources.h"
#include <iostream>

//using namespace std;

char CDialogueLoader::m_luaCode [2048];

CDialogueLoader::CDialogueLoader (const string & filename) :
	mDialogue (filename)
{
	std::vector<CCharacter*> characters;
	Construct( filename.c_str(), characters );
}

CDialogueLoader::CDialogueLoader (const char* filename) :
	mDialogue (filename)
{
	std::vector<CCharacter*> characters;
	Construct( filename, characters );
}

CDialogueLoader::CDialogueLoader (const string & filename, std::vector<CCharacter*> characters) : 
	mDialogue (filename), nCharacters (characters)
{
	Construct( filename.c_str(), characters );
}

CDialogueLoader::CDialogueLoader (const char* filename, std::vector<CCharacter*> characters):
	mDialogue (filename), nCharacters (characters)
{
	Construct( filename, characters );
}

void CDialogueLoader::Construct ( const char* filename, std::vector<CCharacter*> characters )
{
	//pFile = fopen (filename, "rb");
	pFile = Core::Resources::Open (filename, "rb");
	lCurrentDialogue = new COSF_Loader (pFile);

	sIndex = "0";
	sCurrentEntry.level = 0;
	sCurrentEntry.type = mccOSF_entrytype_enum::MCCOSF_ENTRY_NORMAL;
	sCurrentEntry.nextchar = 0;
	sCurrentLine = "";
	sDialogueState = -1;
	sTalker = 0;
	sPrevious = "";

	NextObject();
}

CDialogueLoader::~CDialogueLoader (void)
{
	if (pFile) {
		fclose (pFile);
	}
	pFile = NULL;

	delete_safe( lCurrentDialogue );

	mDialogue = "";
	sIndex = "";
	sDialogueState = 0;
	sPrevious = "";
}


string CDialogueLoader::GetCurrentLine (void)
{
	return sLines[LineIt].line;
}

std::vector<CDialogueLoader::ChoiceStruct> CDialogueLoader::GetChoices (void)
{
	return sOptions;
}

char* CDialogueLoader::GetLua (void)
{
	return m_luaCode;
}

void CDialogueLoader::ReportDecision (short choice)
{
	if (choice >= 0 && choice < (short)sOptions.size())
	{
		SkipTo (sOptions[choice]);

		NextObject();
	}
}

void CDialogueLoader::SkipTo (ChoiceStruct selected)
{
	if (!selected.address.empty())
	{
		string place = "#" + selected.address;

		lCurrentDialogue->SearchToMarker (place.c_str());
		lCurrentDialogue->GetNext (sCurrentEntry);
	}

}

void CDialogueLoader::MakeLines (void)
{
	LineType nextLine;
	nextLine.line = "";
	nextLine.audio = "";
	nextLine.action = "";
	nextLine.expression = "";

	sLines.clear();

	bool firstLine = true;

	// Read in the entire entry
	while (sCurrentEntry.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_END)
	{
		// Audio input
		if (strstr (sCurrentEntry.name, "audio") != NULL)
		{
			nextLine.audio = sCurrentEntry.value;
		}
		// Action input (what is action?) (technically not needed w/ audio, since audio dictates a morph file!)
		else if (strstr (sCurrentEntry.name, "action") != NULL)
		{
			nextLine.action = sCurrentEntry.value;
		}
		// Expression input
		else if (strstr (sCurrentEntry.name, "expression") != NULL)
		{
			nextLine.expression = sCurrentEntry.value;
		}
		// Line input (push onto stack)
		else if ( atoi(sCurrentEntry.name) != 0 )
		{
			// Push onto the list when reading another line (so you may have things out of order, but still in logical order)
			if ( firstLine ) { // Skip first line
				firstLine = false;
			}
			else {
				sLines.push_back(nextLine);

#ifdef _ENGINE_DEBUG
				std::cout << "New Line: " << std::endl
					<< nextLine.line << std::endl
					<< "  audio: 	  " << nextLine.audio << std::endl
					<< "  action:	  " << nextLine.action << std::endl
					<< "  expression: " << nextLine.expression << std::endl;
#endif

				// Clear the line
				nextLine.line = "";
				nextLine.audio = "";
				nextLine.action = "";
				nextLine.expression = "";
			}

			nextLine.line = sCurrentEntry.value;
		}
		
		// Go to the next line
		lCurrentDialogue->GetNext (sCurrentEntry);
	}

	// Push in the last line (doesn't get added in)
	sLines.push_back(nextLine);

#ifdef _ENGINE_DEBUG
	std::cout << "New Line: " << std::endl
		<< nextLine.line << std::endl
		<< "  audio: 	  " << nextLine.audio << std::endl
		<< "  action:	  " << nextLine.action << std::endl
		<< "  expression: " << nextLine.expression << std::endl;
#endif

	LineIt = 0;
}

void CDialogueLoader::MakeChoices (void)
{
	ChoiceStruct blank;
	short level = 0;
	blank.choice = "";
	blank.address = "";
	sOptions.clear();

	m_choiceTimerActive = false;
	m_choiceDefault = -1;

	// Loop through all the choices
	lCurrentDialogue->GetNext (sCurrentEntry);
	level = sCurrentEntry.level;
	do 
	{
		// Found a regular entry
		if (sCurrentEntry.type == mccOSF_entrytype_enum::MCCOSF_ENTRY_NORMAL)
		{
			// Check for property values
			if ( strcmp( sCurrentEntry.name, "timer" ) == 0 )
			{
				m_choiceTimer = (Real) atof( sCurrentEntry.value );
				m_choiceTimerSet = m_choiceTimer;
				m_choiceTimerActive = true;
			}
			else if ( strcmp( sCurrentEntry.name, "default" ) == 0 )
			{
				m_choiceDefault = atoi( sCurrentEntry.value ) - 1;
			}
			// Check for choice value
			else if ( atoi( sCurrentEntry.name ) != 0 )
			{
				blank.choice = sCurrentEntry.value;
				sOptions.push_back (blank);
				blank.choice = "";
			}
		}
		else if (sCurrentEntry.type == mccOSF_entrytype_enum::MCCOSF_ENTRY_OBJECT)
		{
			blank.choice = sCurrentEntry.value;
			lCurrentDialogue->GoInto (sCurrentEntry);
			lCurrentDialogue->GetNext (sCurrentEntry);
			blank.address = sCurrentEntry.value;
			sOptions.push_back (blank);
			blank.choice = "";
			blank.address = "";
		}
		lCurrentDialogue->GetNext (sCurrentEntry);
	}
	while (sCurrentEntry.level >= level);
}

void CDialogueLoader::MakeLua (void)
{
	lCurrentDialogue->GetNext( sCurrentEntry, m_luaCode );
}


void CDialogueLoader::NextObject (void)
{
	// Look for the first object
	//lCurrentDialogue->GetNext( sCurrentEntry, m_luaCode );
	while (sCurrentEntry.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_OBJECT)
	{
		if (strcmp (sCurrentEntry.name, "#ended") == 0 || sCurrentEntry.type == mccOSF_entrytype_enum::MCCOSF_ENTRY_EOF)
		{
			sDialogueState = DIALOGUE_STATE_ENDED;
			break;
		}
		if (strcmp (sCurrentEntry.name, "lua") == 0)
		{
			m_luaState = 0;
			sDialogueState = DIALOGUE_STATE_LUACODE;
			break;
		}
		
		lCurrentDialogue->GetNext( sCurrentEntry, m_luaCode );
	}

	// Check for lines
	if ( strcmp (sCurrentEntry.name, "lines") == 0 )
	{
		char* strPos = strstr( sCurrentEntry.value, "character" );
		if ( strPos )
		{
			sTalker = atoi( (strPos+9) );
			if ( sTalker == 0 ) {
				sDialogueState = DIALOGUE_STATE_PLAYERTALKING;
			}
			else {
				sDialogueState = DIALOGUE_STATE_NPCTALKING;
			}
			lCurrentDialogue->GoInto (sCurrentEntry);
			MakeLines();
		}
	}
	// Check for choice
	else if ( strcmp (sCurrentEntry.name, "choice") == 0 )
	{
		lCurrentDialogue->GoInto (sCurrentEntry);
		MakeChoices();
		sDialogueState = DIALOGUE_STATE_CHOICES;
	}
}

void CDialogueLoader::GoNextLine (void)
{
	//sLines.empty() == false && 
	if ( LineIt < ((short)sLines.size())-1 )
	{
		LineIt++;
	}
	else 
	{
		NextObject();
	}
}

#include "engine-common/Lua/CLuaController.h"

int cont(lua_State *L)
{
    return 0;
}
string lc_target;
int pcallktest( lua_State *L )
{
    int s;
	s = luaL_loadstring(L, lc_target.c_str());
	// Check that the code loaded properly
	if ( s != LUA_OK ) {
		Lua::Controller->ReportErrors( L, s );
		Debug::Console->PrintError( "...error in lua load!" );
	}
    s = lua_pcallk(L, 0, 0, 0, 0, cont);
	// Check for errors caused
	if ( s == LUA_OK ) {
		Debug::Console->PrintWarning( "...routine finished w/o yield?" );
	}
	else {
		Lua::Controller->ReportErrors( L, s );
		Debug::Console->PrintError( "...error in lua!" );
	}
    return 0;
}

void CDialogueLoader::PostLua (void)
{
	//Debug::Console->PrintMessage( "Lua execution...\n" );

	// Load up lua
	//string m_environment;
	//m_environment = "__dialogue_object_";
	//Lua::Controller->LoadLuaFile( m_behavior.c_str(), m_environment.c_str(), m_environment.c_str() );
	//Lua::Controller->SetEnvironment( m_environment.c_str() );
	//Lua::Controller->ResetEnvironment();
	static lua_State* luaThread = NULL; 

	// Todo: Mark this dialogue as the active one for the C hooks

	if ( m_luaState == 0 )
	{
		Debug::Console->PrintMessage( "Lua execution...\n" );

		luaThread = lua_newthread( Lua::Controller->GetState() );
		//Lua::Controller->SetEnvironment( m_environment.c_str() );

		//Lua::Controller->RunLua( m_luaCode );

		//string target = ;
		//luaL_loadstring( Lua::Controller->GetState(), target.c_str() );
		//int s = lua_pcallk( Lua::Controller->GetState(), 0,0,0,0, NULL );

		luaL_loadstring( luaThread, "local pcallktest = ... \r\n pcallktest()" );

		//lc_target = "print(\"HI\"); \r\n yield();\r\n" + string(m_luaCode);

		lc_target = string(m_luaCode);
		lua_pushcfunction( luaThread, pcallktest );
		
		//Lua::Controller->ResetEnvironment();
		m_luaState = 1;
	}
	if ( m_luaState == 1 || m_luaState == 2 )
	{
		int s;
		//Debug::Console->PrintMessage( "...running function...\n" );
		s = lua_resume( luaThread, NULL, (m_luaState==1)?1:0 );

		if ( s == LUA_YIELD ) {
			//Debug::Console->PrintMessage( "yielded...\n" );
			m_luaState = 2;
			return; // hohoho
		}
		else if ( s == LUA_OK ) {
			Debug::Console->PrintWarning( "...routine finished!\n" );
			m_luaState = 3;
		}
		else {
			Lua::Controller->ReportErrors( luaThread, s );
			Debug::Console->PrintError( "...error in lua!" );
			m_luaState = 3;
		}
	}
	if ( m_luaState == 3 )
	{
		// Check if script is finished executing
		sCurrentEntry.name[0] = 0;
		NextObject();
	}
}

void CDialogueLoader::SoundAndFace (void)
{
	/*if ( sDialogueState == DIALOGUE_STATE_PLAYERTALKING )
	{
		if (sLines[LineIt].audio.empty() == false)
		{
			nCharacters[0]->SpeakDialogue (sLines[LineIt].audio);
			sLines[LineIt].audio.clear();
		}
	}
	if ( sDialogueState == DIALOGUE_STATE_NPCTALKING )
	{
		if (sLines[LineIt].audio.empty() == false)
		{
			nCharacters[sTalker]->SpeakDialogue (sLines[LineIt].audio);
			sLines[LineIt].audio.clear();
		}
	}*/
	if ( !sLines[LineIt].audio.empty() )
	{
		nCharacters[sTalker]->SpeakDialogue( sLines[LineIt].audio );
		sLines[LineIt].audio.clear(); // So we play the audio then destroy it so we don't play it again? Alright.
	}

	if ( !sLines[LineIt].expression.empty() || !sLines[LineIt].action.empty() )
	{
		nCharacters[sTalker]->PerformActionList( sLines[LineIt].action );
		nCharacters[sTalker]->PerformExpressionList( sLines[LineIt].expression );

		sLines[LineIt].action.clear(); // So we play the audio then destroy it so we don't play it again? Alright.
		sLines[LineIt].expression.clear(); // So we play the audio then destroy it so we don't play it again? Alright.
	}
}

CCharacter* CDialogueLoader::GetParticipant ( const short n_index )
{
	if ( n_index < 0 || n_index >= (short) nCharacters.size() ) {
		return NULL;
	}
	else {
		return nCharacters[n_index];
	}
}

// Return the index of the current speaker. May not be valid.
short CDialogueLoader::GetCurrentSpeaker ( void ) const
{
	return sTalker;
}

// Choice update step
void CDialogueLoader::ChoiceUpdate ( void )
{
	if ( m_choiceTimerActive && m_choiceDefault != -1 )
	{
		m_choiceTimer -= Time::deltaTime;
		if ( m_choiceTimer <= 0 )
		{
			ReportDecision( m_choiceDefault );
		}
	}
}

bool CDialogueLoader::IsTimedChoice ( void ) const
{
	return ( m_choiceTimerActive && m_choiceDefault != -1 );
}
float CDialogueLoader::CurrentChoiceTimer ( void ) const
{
	return m_choiceTimer/m_choiceTimerSet;
}
short CDialogueLoader::GetDefaultChoice ( void ) const
{
	return m_choiceDefault;
}