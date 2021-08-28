#include "core/os.h" // OS include must be first for console
#include "Console.h"

#include "core/types/types.h"
#include "core/settings/CGameSettings.h"
#include "core/containers/arstring.h"
#include "core/debug.h"

#include "core-ext/threads/naming.h" // TODO: This is an invalid include. Either move this console to core-ext or the naming to core

#include <array>
#include <thread>
#include <atomic>

//===============================================================================================//

// TODO: Make a thread-safe arring container variation

template <typename Type, int Size>
class arRingMultiProduceSingleConsume
{
public:
	explicit arRingMultiProduceSingleConsume ( void )
		: start(0), end_pusher(0), end_popper(0)
	{
	}

	void push ( const Type&& value )
	{
		// Get the new end (can call Push from other threads)
		int oldEnd, newEnd;
		do
		{
			oldEnd = end_pusher;
			newEnd = (oldEnd + 1) % Size;
		} while (!end_pusher.compare_exchange_weak(oldEnd, newEnd));

		// Wait for room in the buffer
		while (newEnd == start)
		{
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}

		// Put new value in
		data[newEnd] = value;

		// Commit the new index to the array
		end_popper.compare_exchange_strong(oldEnd, newEnd);
	}

	void pop ( void )
	{
		ARCORE_ASSERT(!empty());
		start = (start + 1) % Size;
	}

	bool empty ( void )
	{
		return start == end_popper;
	}

	const Type& current ( void )
	{
		return data[start];
	}

	std::atomic_int		start;
	std::atomic_int		end_pusher;
	std::atomic_int		end_popper;
	std::array<Type, Size>
						data;
};

//===============================================================================================//

enum class arMessageType : uint8
{
	kMessage,
	kWarning,
	kError,
};

struct arMessageRequest
{
	arMessageType	type;
	arstring<2047>	message;
};
static_assert(sizeof(arMessageRequest) == 2048);

//===============================================================================================//

static constexpr int	kQueueSize = 16;

debug::ConsoleWindow*	debug::Console = NULL;

static bool				bOutputEnabled = true;
static std::thread		g_OutputThread;
static std::atomic_flag	g_QuitFlag;

arRingMultiProduceSingleConsume<arMessageRequest, kQueueSize>
						g_QueueMessage;

//===============================================================================================//

#ifdef _WIN32
#	if _MSC_VER >= 1900
// Cfile rework for output
class outbuf : public std::streambuf
{
public:
	outbuf()
	{
		setp(0, 0);
	}

	virtual int_type overflow(int_type c = traits_type::eof())
	{
		return fputc(c, stdout) == EOF ? traits_type::eof() : c;
	}
};
#	endif
#endif

//===============================================================================================//

static void PrintOutputRoutine ( void )
{
	core::threads::SetThisThreadName("Console_Output");

	g_QuitFlag.test_and_set(); // Set flag initially so we enter the loop.

	while (g_QuitFlag.test_and_set())
	{
		while (!g_QueueMessage.empty())
		{
		#ifdef _WIN32
			HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			if (g_QueueMessage.current().type == arMessageType::kMessage)
				SetConsoleTextAttribute( lStdHandle, 15 );
			else if (g_QueueMessage.current().type == arMessageType::kWarning)
				SetConsoleTextAttribute( lStdHandle, 14 );
			else if (g_QueueMessage.current().type == arMessageType::kError)
				SetConsoleTextAttribute( lStdHandle, 12 );

			printf("%.*s", 2047, g_QueueMessage.current().message.c_str());

			SetConsoleTextAttribute( lStdHandle, 7 );
		#endif

			g_QueueMessage.pop();
		}

		std::this_thread::yield();
	}
}

//===============================================================================================//

void debug::ConsoleWindow::Init ( bool create_new_window )
{
	// Create a new console
	if ( !Console )
	{
		Console = new ConsoleWindow();
		g_OutputThread = std::thread(PrintOutputRoutine);
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
	else if (create_new_window)
	{
		Console->bCreatedNewOutput = true;

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
	else
	{
		Console->bCreatedNewOutput = false;
	}
}

void debug::ConsoleWindow::Free ( void )
{
	// Kill the output thread
	g_QuitFlag.clear();
	g_OutputThread.join();

	// Close all handles
	if ( Console && Console->bCreatedNewOutput )
	{
		if ( stdin ) fclose( stdin );
		if ( stdout ) fclose( stdout );
		if ( stderr ) fclose( stderr );
	}

	// Kill the console
	if ( Console )
	{
		delete Console;
	}
	Console = NULL;
}


void debug::ConsoleWindow::PrintMessage ( const char* fmt, ... )
{
	arstring<2047> buffer;

	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(buffer.data, 2048, fmt, argptr);
	va_end(argptr);

	g_QueueMessage.push(arMessageRequest{arMessageType::kMessage, std::move(buffer)});
}

void debug::ConsoleWindow::PrintWarning ( const char* fmt, ... )
{
	arstring<2047> buffer;

	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(buffer.data, 2048, fmt, argptr);
	va_end(argptr);

	g_QueueMessage.push(arMessageRequest{arMessageType::kWarning, std::move(buffer)});
}

void debug::ConsoleWindow::PrintError ( const char* fmt, ... )
{
	arstring<2048> buffer;

	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(buffer.data, 2048, fmt, argptr);
	va_end(argptr);

	g_QueueMessage.push(arMessageRequest{arMessageType::kError, std::move(buffer)});
}

void debug::ConsoleWindow::PrintMessage ( const std::string& str )
{
	PrintMessage(str.c_str());
}
void debug::ConsoleWindow::PrintWarning ( const std::string& str )
{
	PrintWarning(str.c_str());
}
void debug::ConsoleWindow::PrintError ( const std::string& str )
{
	PrintError(str.c_str());
}

void debug::ConsoleWindow::DisableOutput ( void )
{
	bOutputEnabled = false;
}
void debug::ConsoleWindow::EnableOutput ( void )
{
	bOutputEnabled = true;
}