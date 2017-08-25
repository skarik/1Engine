
#ifndef _C_CORE_DEVELOPER_CONSOLE_H_
#define _C_CORE_DEVELOPER_CONSOLE_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <functional>

#include "core/types/types.h"
#include "engine/behavior/CGameBehavior.h"
using std::string;

class CInputControl;
class CDeveloperConsoleUI;
class CDeveloperCursor;

//class CTimeProfilerUI;

namespace engine
{
	class CDeveloperConsole : public CGameBehavior
	{
		ClassName( "CDeveloperConsole" );
	public:
		ENGINE_API explicit	CDeveloperConsole ( void );
		ENGINE_API			~CDeveloperConsole ( void );

		void				Update ( void );
		void				PostUpdate ( void );

		ENGINE_API void		RunCommand ( string const& );
		ENGINE_API void		RunExternal( string const& );
	
		// Get the current console command (for GUI)
		ENGINE_API string&	GetCommandString ( void );
		// Get the list of matching commands (for GUI)
		ENGINE_API std::vector<string>&	GetMatchingCommands ( void );
		// Get if console open (for GUI)
		ENGINE_API bool		GetIsOpen ( void ) const;

		// Add function to the console when command is called
		ENGINE_API void		AddConsoleFunc ( string const&, int (*fnc) ( string const& ) );
		// Add Lua hook
		ENGINE_API void		AddConsoleHook ( string const&, string const& );
		// Add console variable
		ENGINE_API void		AddConsoleVariable ( string const&, int* );
		ENGINE_API void		AddConsoleVariable ( string const&, Real* );

		// Add a manual match
		ENGINE_API void		AddConsoleMatch ( string const& );
	private:
		//CDeveloperConsoleUI*	mUI;
		//CDeveloperCursor*		mCursor;
		CInputControl*			mControl;
		//CTimeProfilerUI*		mTimeProfilerUI;

		//void			GenerateDefaultFunctionList ( void );
		bool			RunLastCommand ( void );

		string		sLastCommand;
		bool		bAcceptingCommands;
		bool		bRunCommand;

		enum eConsoleFunctionType : int8_t
		{
			CONFUNC_STRINGREF,
			//CONFUNC_CHARPTR
		};
		struct consoleFnc_t {
			int (*fnc) ( string const& );
			int8_t	type;
		};
		enum eConsolVariableType : int8_t
		{
			CONVAR_INT,
			CONVAR_REAL,
		};
		struct consoleVar_t {
			void*	var;
			int8_t	type;
		};
		std::map<string,consoleFnc_t>	functionList;
		std::map<string,consoleVar_t>	variableList;
		std::list<string>				manualMatchList;

		// Regenerates the matching command list
		void			MatchCommands ( void );
		std::vector<string>		matchingCommands;

		// Previous command list
		std::vector<string>		previousCommands;
		int					iPreviousCommandSelection;
	};

	ENGINE_API extern CDeveloperConsole* Console;
};

#endif//_C_CORE_DEVELOPER_CONSOLE_H_