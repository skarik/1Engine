
#ifndef _RENDERER_EXCEPTIONS_H_
#define _RENDERER_EXCEPTIONS_H_

#include "core/exceptions/exceptions.h"

namespace Renderer
{
	class GLStateException : public std::runtime_error
	{
	public:
		GLStateException ( void ) : runtime_error( "Error in the GL state." )
		{
			;
		}
	};
	class TooManyPassesException : public std::runtime_error
	{
	public:
		TooManyPassesException ( void ) : runtime_error( "Too many passes in the material to handle." )
		{
			;
		}
	};
	class InvalidPassException : public std::runtime_error
	{
	public:
		InvalidPassException ( void ) : runtime_error( "Invalid pass in the material." )
		{
			;
		}
	};
	class InvalidOperationException : public std::runtime_error
	{
	public:
		InvalidOperationException ( void ) : runtime_error( "Invalid operation in renderer." )
		{
			;
		}
	};
}

#endif//_RENDERER_EXCEPTIONS_H_