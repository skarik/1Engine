//===============================================================================================//
//
//		exceptions.h
//
// Provides silly exceptions with great meaning under the core:: namespace, as well as the
//	EXCEPTION_CATCH_BEGIN and EXCEPTION_CATCH_END macros.
//
//===============================================================================================//
#ifndef CORE_EXCEPTIONS_H_
#define CORE_EXCEPTIONS_H_

#include "core/types/types.h"
#include <exception>
#include <stdexcept>

#if !defined(_ENGINE_DEBUG)
#	define	EXCEPTION_CATCH_BEGIN
#	define	EXCEPTION_CATCH_END
#else
#	define	EXCEPTION_CATCH_BEGIN try {
#	define	EXCEPTION_CATCH_END } catch ( const std::exception& e ) { debug::Console->PrintWarning( e.what() ); __debugbreak(); }
#endif

namespace core
{
	class NotYetImplementedException : public std::runtime_error
	{
	public:
		NotYetImplementedException ( void ) : runtime_error( "Functionality not yet implemented" )
		{
			;
		}
		NotYetImplementedException ( const char* customMessage ) : runtime_error( customMessage )
		{
			;
		}
	};


	class NoWorldException : public std::runtime_error
	{
	public:
		NoWorldException ( void ) : runtime_error( "No world found" )
		{
			;
		}
	};

	class NoTerrainException : public std::runtime_error
	{
	public:
		NoTerrainException ( void ) : runtime_error( "No terrain found" )
		{
			;
		}
	};

	class NullReferenceException : public std::runtime_error
	{
	public:
		NullReferenceException ( void ) : runtime_error( "Null pointer reference" )
		{
			;
		}
	};

	class InvalidCallException : public std::runtime_error
	{
	public:
		InvalidCallException ( void ) : runtime_error( "Invalid function call" )
		{
			;
		}
	};

	class DeprecatedCallException : public std::runtime_error
	{
	public:
		DeprecatedCallException ( void ) : runtime_error( "Deprecated function call" )
		{
			;
		}
	};
	class DeprecatedFeatureException : public std::runtime_error
	{
	public:
		DeprecatedFeatureException ( void ) : runtime_error( "Deprecated feature used" )
		{
			;
		}
	};

	class CorruptedDataException : public std::runtime_error
	{
	public:
		CorruptedDataException ( void ) : runtime_error( "Data corruption" )
		{
			;
		}
	};

	class OutOfMemoryException : public std::runtime_error
	{
	public:
		OutOfMemoryException ( void ) : runtime_error( "Out of memory" )
		{
			;
		}
	};

	class MemoryLeakException : public std::runtime_error
	{
	public:
		MemoryLeakException ( void ) : runtime_error( "Memory leak detected." )
		{
			;
		}
	};

	class InvalidArgumentException : public std::runtime_error
	{
	public:
		InvalidArgumentException ( void ) : runtime_error( "Invalid argument passed in" )
		{
			;
		}
	};

	class InvalidInstantiationException : public std::runtime_error
	{
	public:
		InvalidInstantiationException ( void ) : runtime_error( "Invalid instance created" )
		{
			;
		}
	};

	class MissingFileException : public std::runtime_error
	{
	public:
		MissingFileException ( void ) : runtime_error( "File could not be found!" )
		{
			;
		}
	};

	class MissingDataException : public std::runtime_error
	{
	public:
		MissingDataException ( void ) : runtime_error( "Data culd not be found" )
		{
			;
		}
	};

	class YouSuckException : public std::runtime_error
	{
	public:
		YouSuckException ( void ) : runtime_error( "You really suck." )
		{
			;
		}
	};
}

#endif//CORE_EXCEPTIONS_H_