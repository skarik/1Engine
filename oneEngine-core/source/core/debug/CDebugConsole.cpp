
// System specific includes
#include "core/os.h"

// Includes
#include "CDebugConsole.h"
#include "core/types/types.h"
#include "core/settings/CGameSettings.h"

// Static variables
debug::CDebugConsole*	debug::Console							= NULL;
bool					debug::CDebugConsole::bOutputEnabled	= true;

#ifdef _WIN32
#	if _MSC_VER >= 1900
// Cfile rework for output
class outbuf : public std::streambuf {
public:
	outbuf() {
		setp(0, 0);
	}

	virtual int_type overflow(int_type c = traits_type::eof()) {
		return fputc(c, stdout) == EOF ? traits_type::eof() : c;
	}
};
#	endif
#endif

// Static functions
// Debug Console Init
void debug::CDebugConsole::Init ( void )
{
	// Create a new console
	if ( !Console ) {
		Console = new CDebugConsole();
	}

	if ( CGameSettings::Active()->b_dbg_logToFile )
	{
		// Open two log files
		freopen( ".game/output0.log", "wb", stdout );
		freopen( ".game/output2.log", "wb", stderr );
		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();
	}
	else
	{
#	ifdef _WIN32

		// Get a console for this program
		AllocConsole();

#		if _MSC_VER < 1900

		int hConHandle;
		long lStdHandle;
		FILE *fp;

		// redirect unbuffered STDOUT to the console
		lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "w" );
		*stdout = *fp;
		setvbuf( stdout, NULL, _IONBF, 0 );
		// redirect unbuffered STDIN to the console
		lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "r" );
		*stdin = *fp;
		setvbuf( stdin, NULL, _IONBF, 0 );
		// redirect unbuffered STDERR to the console
		lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( hConHandle, "w" );
		*stderr = *fp;
		setvbuf( stderr, NULL, _IONBF, 0 );

		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();

#		else

		// Redirect STDOUT to console
		FILE* pCout;
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		// Redirect STDIN from console
		FILE* pCin;
		freopen_s(&pCin,  "CONIN$",  "r", stdin );
		// Redirect STDERR to console
		FILE* pCerr;
		freopen_s(&pCerr, "CONOUT$", "w", stderr);

		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
		std::ios::sync_with_stdio();

#		endif

		// Move console to the side
		HWND hConsole;
		hConsole = GetConsoleWindow();
		SetWindowPos(hConsole, 0, 0, 16, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

#	elif 
	/// use openpty to create a terminal window and redirect output to it
#	endif
	}
}

// Free
void debug::CDebugConsole::Free ( void )
{
	// Kill the console
	if ( Console ) {
		delete Console;
	}
	Console = NULL;
	// Close all handles
	if ( stdin ) fclose( stdin );
	if ( stdout ) fclose( stdout );
	if ( stderr ) fclose( stderr );
}


// Output functions
void debug::CDebugConsole::PrintMessage ( const string& sOut )
{
#ifdef _WIN32
	HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute( lStdHandle, 15 );

	if ( bOutputEnabled )
		std::cout << sOut;

	SetConsoleTextAttribute( lStdHandle, 7 );
#endif
}

void debug::CDebugConsole::PrintWarning ( const string& sOut )
{
#ifdef _WIN32
	HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute( lStdHandle, 14 );

	if ( bOutputEnabled )
		std::cout << sOut;

	SetConsoleTextAttribute( lStdHandle, 7 );
#endif
}

void debug::CDebugConsole::PrintError ( const string& sOut )
{
#ifdef _WIN32
	HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute( lStdHandle, 12 );

	if ( bOutputEnabled )
		std::cout << sOut;

	SetConsoleTextAttribute( lStdHandle, 7 );
#endif
}


void debug::CDebugConsole::DisableOutput ( void )
{
	bOutputEnabled = false;
}
void debug::CDebugConsole::EnableOutput ( void )
{
	bOutputEnabled = true;
}