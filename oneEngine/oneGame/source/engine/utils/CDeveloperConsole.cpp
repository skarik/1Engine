
//#include "Console.h"
#include "core/debug/Console.h"
#include "CDeveloperConsole.h"
//#include "CInput.h"
#include "core/input/CInput.h"
#include "core-ext/input/CInputControl.h"
//#include "CTimeProfilerUI.h"
#include <string>
#include <algorithm>
#include <cctype>
//#include "CGameSettings.h"

using namespace engine;
using std::cin;
using std::string;

// Global pointer to console
CDeveloperConsole*	engine::Console = NULL;
//CDeveloperCursor*	ActiveCursor = NULL;

// Constructor and Destructor
CDeveloperConsole::CDeveloperConsole ( void ) 
	: CGameBehavior ()
{
	if ( Console != NULL )
	{
		CGameBehavior::DeleteObject( this );
		this->active = false;
	}
	else
	{
		sLastCommand = "";
		bAcceptingCommands = false;
		bRunCommand = false;
		iPreviousCommandSelection = 0;

		Console		= this;

		// Set up input control
		mControl = new CInputControl( this );

		SetPersistence(true);
	}
}

CDeveloperConsole::~CDeveloperConsole ( void )
{
	if ( Console == this ) {
		Console		= NULL;
	}
	
	delete_safe( mControl );
}


void CDeveloperConsole::RunCommand ( const string& cmd )
{
	sLastCommand = cmd;
	RunLastCommand();
	sLastCommand = "";
}


// Get the current console command (for GUI)
string&	CDeveloperConsole::GetCommandString ( void )
{
	return sLastCommand;
}
// Get the list of matching commands (for GUI)
std::vector<string>&	CDeveloperConsole::GetMatchingCommands ( void )
{
	return matchingCommands;
}
// Get if console open (for GUI)
bool CDeveloperConsole::GetIsOpen ( void ) const
{
	return bAcceptingCommands;
}
//#include "CTimeProfiler.h"
//#include "RrShaderManager.h"
void CDeveloperConsole::Update ( void )
{
	// Execute the command if accepting
	if ( bRunCommand )
	{
		previousCommands.push_back( sLastCommand );
		if ( previousCommands.size() > 14 ) {
			previousCommands.erase( previousCommands.begin() );
		}
		RunLastCommand();
		bRunCommand = false;
		sLastCommand = "";
	}

	/*if ( Input::Keydown( Keys.F9 ) )//if ( core::Input::keydown[VK_F9] )
	{
		std::cout << "Recompiling all shaders...(this may take a while)..." << std::endl;
		ShaderManager.RecompileAll();
	}*/
	/*if ( Input::Keydown( Keys.F8 ) )
	{
		CGameSettings::Active()->b_dbg_ro_EnableSkinning = !CGameSettings::Active()->b_dbg_ro_EnableSkinning;
	}*/

	/*if ( Input::Keydown( Keys.F8 ) )
	{
		TimeProfiler.ToggleUIVisibility
	}*/

	/*if ( Input::Keydown( Keys.F6 ) )
	{
		mTimeProfilerUI->visible = !mTimeProfilerUI->visible;
	}*/

}

void CDeveloperConsole::PostUpdate ( void )
{
	// If input says we hit tilde, toggle command acceptance state and eat inputs
	if ( core::Input::Keydown(core::kVkTilde) ) {
		bAcceptingCommands = !bAcceptingCommands;
		if ( bAcceptingCommands ) {
			mControl->Capture();
		}
		else {
			mControl->Release();
		}
	}
	
	if ( bAcceptingCommands )
	{
		// Get input
		const auto& inputString = core::Input::FrameInputString();
		for (const auto& input : inputString)
		{
			if ( input && isprint(input) )
			{
				sLastCommand += input;
				// Reset the command selection
				iPreviousCommandSelection = -1;
				// Regenerate the matching commands list
				MatchCommands();
			}
			if ( input == core::kVkBackspace )
			{
				// Reset the command selection
				iPreviousCommandSelection = -1;
				// Subtract a character
				sLastCommand = sLastCommand.substr(0,sLastCommand.length()-1);
				// Regenerate the matching commands list
				MatchCommands();
			}
		}
		if ( core::Input::Keydown( core::kVkTab ) ) {
			if ( iPreviousCommandSelection == -1 ) {
				// Autocomplete
				if ( !matchingCommands.empty() ) {
					sLastCommand = matchingCommands[0] + " ";
				}
			}
		}
		// Check for press up to go through previous commands
		if ( core::Input::Keydown( core::kVkUp ) ) {
			iPreviousCommandSelection -= 1;
			if ( iPreviousCommandSelection < -1 ) {
				iPreviousCommandSelection = ((int)previousCommands.size())-1;
			}
			if ( iPreviousCommandSelection == -1 ) {
				sLastCommand = "";
			}
			else {
				sLastCommand = previousCommands[iPreviousCommandSelection];
			}
		}
		// Run command otherwise
		if ( core::Input::Keydown( core::kVkReturn ) ) {
			bRunCommand = true;
		}
	}

	//mUI->visible = bAcceptingCommands;
	//mUI->visible = true;
}
void CDeveloperConsole::MatchCommands ( void )
{
	auto varListIterator = variableList.begin();
	auto fncListIterator = functionList.begin();
	auto strListIterator = manualMatchList.begin();
	bool addVar = false;
	bool addFnc = false;
	bool addStr = false;

	// List of strs that we can add
	std::list<std::pair<string,int>> strs_to_add;

	matchingCommands.clear();
	while ( (varListIterator != variableList.end() || fncListIterator != functionList.end() || strListIterator != manualMatchList.end()) && (matchingCommands.size() < 13) )
	{
		// Get the next matching item from each list.
		while ( !addVar && varListIterator != variableList.end() ) {
			if ( varListIterator->first.find( sLastCommand ) != string::npos ) {
				addVar = true;
				strs_to_add.push_back( std::pair<string,int>(varListIterator->first,0) );
			}
			else {
				++varListIterator;
			}
		}
		while ( !addFnc && fncListIterator != functionList.end() ) {
			if ( fncListIterator->first.find( sLastCommand ) != string::npos ) {
				addFnc = true;
				strs_to_add.push_back( std::pair<string,int>(fncListIterator->first,1) );
			}
			else {
				++fncListIterator;
			}
		}
		while ( !addStr && strListIterator != manualMatchList.end() ) {
			if ( strListIterator->find( sLastCommand ) != string::npos ) {
				addStr = true;
				strs_to_add.push_back( std::pair<string,int>(*strListIterator,2) );
			}
			else {
				++strListIterator;
			}
		}

		// Based on a quick priority check, perform the add
		if ( !strs_to_add.empty() )
		{
			strs_to_add.sort();

			matchingCommands.push_back(strs_to_add.back().first);
			// Reset that 
			switch ( strs_to_add.back().second )
			{
			case 0: addVar = false; if ( varListIterator != variableList.end() ) ++varListIterator; break;
			case 1: addFnc = false; if ( fncListIterator != functionList.end() ) ++fncListIterator; break;
			case 2: addStr = false; if ( strListIterator != manualMatchList.end() ) ++strListIterator; break;
			}
			strs_to_add.pop_back();
		}
	}
	// End add loop

	// Finish off with silly hacks.
	if (sLastCommand.find("what is a man") != string::npos)
	{
		matchingCommands.push_back("a miserable little pile of secrets");
	}
	else if (sLastCommand.find("impulse ") == 0)
	{
		if (sLastCommand == "impulse 101")
		{
			matchingCommands.push_back("Gordon, get away from the beam!");
		}
		else
		{
			matchingCommands.push_back("try cmdc or cmds");
		}
	}
	else if (sLastCommand.find("        ") == 0)
	{
		matchingCommands.push_back("Maybe you should take a break. Pop open a beer and do something else.");
	}
}


// Add a function to the list
void CDeveloperConsole::AddConsoleFunc ( string const& name, int (*fnc) ( string const& ) )
{
	consoleFnc_t newFn;
	newFn.fnc	= fnc;
	newFn.type	= 0;
	functionList[name] = newFn;
}
// Add variables to the list
void CDeveloperConsole::AddConsoleVariable( string const& name, int* var )
{
	consoleVar_t newVr;
	newVr.var	= (void*)var;
	newVr.type	= 0;
	variableList[name] = newVr;
}
void CDeveloperConsole::AddConsoleVariable( string const& name, Real* var )
{
	consoleVar_t newVr;
	newVr.var	= (void*)var;
	newVr.type	= 1;
	variableList[name] = newVr;
}
// Add manual match
void CDeveloperConsole::AddConsoleMatch( string const& name )
{
	if ( std::find(manualMatchList.begin(),manualMatchList.end(), name) == manualMatchList.end() )
	{
		manualMatchList.push_back(name);
	}
	else
	{
		debug::Console->PrintError("Double console link on \"" + name + "\"\n");
	}
}

// Parse the command and actually run it
bool CDeveloperConsole::RunLastCommand ( void )
{
	string cmd = sLastCommand;
	{
		auto suffix = cmd.find_last_not_of(' ');
		if ( suffix != cmd.npos ) 
		{
			cmd.erase(suffix+1);         //surfixing spaces
		}
	}

	// Need to get the actual command and the arguments
	string verb = cmd;
	string args;
	while ( (verb[0] == ' ') && (verb.length() > 0) ) {
		verb = verb.substr(1);
	}
	args = verb;
	verb = verb.substr( 0,verb.find(' ') );
	args = args.substr( verb.length() );
	while ( (args[0] == ' ') && (args.length() > 0) ) {
		args = args.substr(1);
	}

	if ( verb.length() <= 0 ) {
		return false;
	}

	// Search the function list for the command
	if ( functionList.find(verb) != functionList.end() )
	{
		debug::Console->PrintMessage( cmd );
		std::cout << std::endl;
		functionList[verb].fnc( args );
	}
	// Search the variable list for the command
	else if ( variableList.find(verb) != variableList.end() )
	{
		// If argument list is empty, then display current value
		if ( args.length() <= 0 ) {
			debug::Console->PrintMessage( verb );
			debug::Console->PrintMessage( " = " );
			int type = variableList[verb].type;
			if ( type == 0 ) {
				std::cout << (*((int*)(variableList[verb].var)));
			}
			else if ( type == 1 ) {
				std::cout << (*((Real*)(variableList[verb].var)));
			}
			std::cout << std::endl;
		}
		else
		{
			debug::Console->PrintMessage( cmd );
			int type = variableList[verb].type;
			if ( type == 0 ) {
				(*((int*)(variableList[verb].var))) = atoi( args.c_str() );
			}
			else if ( type == 1 ) {
				(*((Real*)(variableList[verb].var))) = (Real)atof( args.c_str() );
			}
		}
	}
	else // Print that cannot find values
	{
		debug::Console->PrintError( verb );
		debug::Console->PrintError( ": command doesn't exist\n" );
	}
	return true;
}
