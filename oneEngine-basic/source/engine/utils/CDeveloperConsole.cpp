
//#include "CDebugConsole.h"
#include "core/debug/CDebugConsole.h"
#include "CDeveloperConsole.h"
//#include "CInput.h"
#include "core/input/CInput.h"
#include "core-ext/input/CInputControl.h"
//#include "CTimeProfilerUI.h"

//#include "CGameSettings.h"

using namespace Engine;
using std::cin;
using std::string;

// Global pointer to console
CDeveloperConsole*	Engine::Console = NULL;
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

		//GenerateDefaultFunctionList();

		/*mUI = new CDeveloperConsoleUI ();
		mCursor = new CDeveloperCursor ();
		ActiveCursor = mCursor;*/

		// Create time profiler
		/*mTimeProfilerUI = new CTimeProfilerUI ();
		mTimeProfilerUI->visible = false;*/

		// Set up input control
		mControl = new CInputControl( this );
	}
}

CDeveloperConsole::~CDeveloperConsole ( void )
{
	/*delete mUI;
	mUI = NULL;
	delete mCursor;
	mCursor = NULL;
	delete mTimeProfilerUI;
	mTimeProfilerUI = NULL;*/

	if ( Console == this ) {
		Console		= NULL;
	}
	//ActiveCursor= NULL;

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
//#include "glShaderManager.h"
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

	/*if ( Input::Keydown( Keys.F9 ) )//if ( CInput::keydown[VK_F9] )
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
	if ( Input::Keydown('`') ) {
		bAcceptingCommands = !bAcceptingCommands;
		if ( bAcceptingCommands ) {
			mControl->Capture();
		}
		else {
			mControl->Release();
		}
	}
	
	if ( bAcceptingCommands ) {
		// Get input
		unsigned char input = Input::GetTypeChar();
		if ( input ) {
			sLastCommand += input;
			// Reset the command selection
			iPreviousCommandSelection = -1;
			// Regenerate the matching commands list
			MatchCommands();
		}
		if ( Input::Keydown( Keys.Backspace ) ) {
			// Reset the command selection
			iPreviousCommandSelection = -1;
			// Subtract a character
			sLastCommand = sLastCommand.substr(0,sLastCommand.length()-1);
		}
		// Check for press up to go through previous commands
		if ( Input::Keydown( Keys.Up ) ) {
			iPreviousCommandSelection += 1;
			if ( iPreviousCommandSelection >= (int)previousCommands.size() ) {
				iPreviousCommandSelection = -1;
			}
			if ( iPreviousCommandSelection == -1 ) {
				sLastCommand = "";
			}
			else {
				sLastCommand = previousCommands[iPreviousCommandSelection];
			}
		}
		// Run command otherwise
		if ( Input::Keydown( Keys.Return ) ) {
			bRunCommand = true;
		}
	}

	//mUI->visible = bAcceptingCommands;
	//mUI->visible = true;
}
void CDeveloperConsole::MatchCommands ( void )
{
	std::map<string,consoleVar_t>::iterator varListIterator = variableList.begin();
	std::map<string,consoleFnc_t>::iterator fncListIterator = functionList.begin();
	bool addVar = false;
	bool addFnc = false;

	matchingCommands.clear();
	while ( (varListIterator != variableList.end() || fncListIterator != functionList.end()) && (matchingCommands.size() < 13) )
	{
		// Get the next matching item from the variable list and function list.
		while ( varListIterator != variableList.end() && !addVar ) {
			if ( varListIterator->first.find( sLastCommand ) != string::npos ) {
				addVar = true;
			}
			else {
				++varListIterator;
			}
		}
		while ( fncListIterator != functionList.end() && !addFnc ) {
			if ( fncListIterator->first.find( sLastCommand ) != string::npos ) {
				addFnc = true;
			}
			else {
				++fncListIterator;
			}
		}
		if ( addFnc && addVar )
		{
			// Figure out which to add based on comparison
			if ( varListIterator->first < fncListIterator->first ) {
				matchingCommands.push_back( varListIterator->first );
				addVar = false;
				++varListIterator;
			}
			else {
				matchingCommands.push_back( fncListIterator->first );
				addFnc = false;
				++fncListIterator;
			}
		}
		else if ( addVar )
		{
			matchingCommands.push_back( varListIterator->first );
			addVar = false;
			++varListIterator;
		}
		else if ( addFnc )
		{
			matchingCommands.push_back( fncListIterator->first );
			addFnc = false;
			++fncListIterator;
		}
	}
	// End add loop
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
void CDeveloperConsole::AddConsoleVariable( string const& name, ftype* var )
{
	consoleVar_t newVr;
	newVr.var	= (void*)var;
	newVr.type	= 1;
	variableList[name] = newVr;
}

// Parse the command and actually run it
bool CDeveloperConsole::RunLastCommand ( void )
{
	string cmd = sLastCommand;

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
		Debug::Console->PrintMessage( cmd );
		std::cout << std::endl;
		functionList[verb].fnc( args );
	}
	// Search the variable list for the command
	else if ( variableList.find(verb) != variableList.end() )
	{
		// If argument list is empty, then display current value
		if ( args.length() <= 0 ) {
			Debug::Console->PrintMessage( verb );
			Debug::Console->PrintMessage( " = " );
			int type = variableList[verb].type;
			if ( type == 0 ) {
				std::cout << (*((int*)(variableList[verb].var)));
			}
			else if ( type == 1 ) {
				std::cout << (*((ftype*)(variableList[verb].var)));
			}
			std::cout << std::endl;
		}
		else
		{
			Debug::Console->PrintMessage( cmd );
			int type = variableList[verb].type;
			if ( type == 0 ) {
				(*((int*)(variableList[verb].var))) = atoi( args.c_str() );
			}
			else if ( type == 1 ) {
				(*((ftype*)(variableList[verb].var))) = (ftype)atof( args.c_str() );
			}
		}
	}
	else // Print that cannot find values
	{
		Debug::Console->PrintError( verb );
		Debug::Console->PrintError( ": command doesn't exist\n" );
	}
	return true;
}