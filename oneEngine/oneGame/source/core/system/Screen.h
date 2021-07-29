//===============================================================================================//
//
//		core/system/Screeen.h : Global screen information.
//
//===============================================================================================//
#ifndef ENGINE_SCREEN_H_
#define ENGINE_SCREEN_H_

#include "core/types/types.h"
#include "core/math/vect2d_template.h"

//	class ArScreen
// A screen corresponds to an output display. They are directly related to windows and are managed by them.
class ArScreen
{
public:
	CORE_API uint		GetWidth ( void ) const
		{ return width; }
	CORE_API uint		GetHeight ( void ) const
		{ return height; }
	CORE_API bool		GetFocused ( void ) const
		{ return focused; }

	CORE_API float		GetAspectRatio ( void ) const
		{ return width / (float)height; }
	CORE_API Vector2i	GetSize ( void ) const
		{ return Vector2i(width, height); }

	CORE_API void		SetIndex ( uint index )
	{
		this->index = index;
	}
	CORE_API void		SetSize ( uint width, uint height )
	{
		this->width = width;
		this->height = height;
	}
	CORE_API void		SetFocused ( bool focused );

private:
	uint				width;
	uint				height;
	bool				focused;
	uint				index;
};

namespace core
{
	CORE_API ArScreen&	GetScreen ( const int index );
	CORE_API ArScreen&	GetFocusedScreen ( void );
	CORE_API void		SetScreenCount ( const int count );
}

#endif//ENGINE_SCREEN_H_