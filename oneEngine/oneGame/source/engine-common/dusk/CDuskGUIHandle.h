
#ifndef _C_DUSK_GUI_HANDLE_H_
#define _C_DUSK_GUI_HANDLE_H_

// Includes
#include "core/math/Rect.h"
#include <string>

class CDuskGUI;
class CDuskGUIElement;

// Start Namespace
namespace Dusk
{
	using std::string;

	class Handle
	{
	public:
		ENGCOM_API Handle ( void ) : index(0) {}
		ENGCOM_API Handle ( int val ) : index(val) {}

		ENGCOM_API Handle& operator= ( int const &rhs ) { index = rhs; return *this; }
		ENGCOM_API bool operator== ( int const &rhs ) { return index==rhs; }
		ENGCOM_API bool operator!= ( int const &rhs ) { return index!=rhs; }
		ENGCOM_API bool operator>= ( int const &rhs ) { return index>=rhs; }
		ENGCOM_API bool operator<= ( int const &rhs ) { return index<=rhs; }
		ENGCOM_API bool operator> ( int const &rhs ) { return index>rhs; }
		ENGCOM_API bool operator< ( int const &rhs ) { return index<rhs; }
		ENGCOM_API bool operator== ( Handle const &rhs ) { return index==rhs.index; }
		ENGCOM_API bool operator!= ( Handle const &rhs ) { return index!=rhs.index; }
		ENGCOM_API bool operator>= ( Handle const &rhs ) { return index>=rhs.index; }
		ENGCOM_API bool operator<= ( Handle const &rhs ) { return index<=rhs.index; }
		ENGCOM_API bool operator> ( Handle const &rhs ) { return index>rhs.index; }
		ENGCOM_API bool operator< ( Handle const &rhs ) { return index<rhs.index; }
		ENGCOM_API operator const int() const { return index; }
		ENGCOM_API operator const unsigned int() const { return (unsigned int)index; }
		ENGCOM_API CDuskGUIElement* operator*() const;
		ENGCOM_API CDuskGUIElement* operator->() const;

		ENGCOM_API void	SetVisible ( const bool=true );
		ENGCOM_API void	SetRect ( const Rect& );
		ENGCOM_API void	SetText ( const string& );
		ENGCOM_API void	SetParent ( const Handle& );
		ENGCOM_API void	ToggleVisibility ( void );

		ENGCOM_API Rect	GetRect ( void );
		ENGCOM_API bool	GetClicked ( void );
		ENGCOM_API bool	GetMouseOver ( void );
		ENGCOM_API bool	GetButtonClicked ( void );
		ENGCOM_API int	GetDialogueResponse ( void );
	private:
		int index;
	};

	extern CDuskGUI* activeGUI;

};
// End Namespace

#endif//_C_DUSK_GUI_HANDLE_H_