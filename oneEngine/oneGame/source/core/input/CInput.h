

#ifndef _C_GAME_KEYBOARD_MOUSE_INPUT_
#define _C_GAME_KEYBOARD_MOUSE_INPUT_

#include "core/types.h"

CORE_API extern unsigned char _inputtable [256];

namespace InputControl
{
	class CXBoxController;
}

class CInput
{
private:
	CORE_API static CInput*	Active;
public:
	CORE_API static void	Initialize ( void );
	CORE_API static void	PreUpdate ( void );
	CORE_API static void	Update ( void );
	CORE_API static void	Reset ( void );
	CORE_API static void	Free ( void );
private:
	bool	key[256];
	bool	keydown[256];
	bool	keyup[256];

	void	_Update ( void );
public:
	static inline void		_key ( unsigned char const keycode_ascii, bool state );
	static inline void		_keydown ( unsigned char const keycode_ascii, bool state );
	static inline void		_keyup ( unsigned char const keycode_ascii, bool state );

	CORE_API static bool	Keypress ( unsigned char const keycode_ascii );
	CORE_API static bool	Key ( unsigned char const keycode_ascii );
	CORE_API static bool	Keydown ( unsigned char const keycode_ascii );
	CORE_API static bool	Keyup ( unsigned char const keycode_ascii );
	CORE_API static bool	KeypressAny ( void );
	CORE_API static bool	KeydownAny ( void );
	CORE_API static bool	KeyupAny ( void );

	CORE_API static unsigned char	GetTypeChar ( void );
	
	CORE_API static void		SetMouseSensitivity ( const Real sensitivity );
	CORE_API static Real		GetMouseSensitivity ( void );
private:
	float mouseX;
	float mouseY;
	int sysMouseX;
	int sysMouseY;
	float deltaMouseX;
	float deltaMouseY;
	int rawDeltaMouseX;
	int rawDeltaMouseY;
	int prevRawDeltaMouseX;
	int prevRawDeltaMouseY;
	float mouseSensitivityX;
	float mouseSensitivityY;
	int currMouseW;
	int deltaMouseW;
	bool mouse[4];
	bool mousedown[4];
	bool mouseup[4];

	int prevMouseW;

public:
	static inline void		_sysMouseX ( const int mousex );
	static inline void		_sysMouseY ( const int mousey );
	static inline void		_addRawMouseX ( const int deltamousex );
	static inline void		_addRawMouseY ( const int deltamousey );
	static inline void		_currMouseW ( const int mousew );

	static inline void		_mouse ( const int mousebutton_id, bool state );
	static inline void		_mousedown ( const int mousebutton_id, bool state );
	static inline void		_mouseup ( const int mousebutton_id, bool state );

	CORE_API static float	MouseX ( void );
	CORE_API static float	MouseY ( void );

	CORE_API static int		SysMouseX ( void );
	CORE_API static int		SysMouseY ( void );

	CORE_API static float	DeltaMouseX ( void );
	CORE_API static float	DeltaMouseY ( void );
	CORE_API static int		DeltaMouseW ( void );

	CORE_API static bool	Mouse ( const int mousebutton_id );
	CORE_API static bool	MouseDown ( const int mousebutton_id );
	CORE_API static bool	MouseUp ( const int mousebutton_id );

public:
	CORE_API static InputControl::CXBoxController* xboxControl;
public:
	//virtual void			Startup ( void ) =0;

	//static int prevMouseX;
	//static int prevMouseY;

	enum eMB_PRESS_TYPES : int
	{
		MBLeft	= 0,
		MBRight	= 1,
		MBMiddle= 2,
		MBXtra	= 3
	};
private:
	static void	setInputTable ( void );
};
typedef CInput Input;

inline void		CInput::_key ( unsigned char const keycode_ascii, bool state ) {
	Active->key[keycode_ascii] = state;
}
inline void		CInput::_keydown ( unsigned char const keycode_ascii, bool state ) {
	Active->keydown[keycode_ascii] = state;
}
inline void		CInput::_keyup ( unsigned char const keycode_ascii, bool state ) {
	Active->keyup[keycode_ascii] = state;
}

inline void		CInput::_sysMouseX ( const int mousex ) {
	Active->sysMouseX = mousex;
}
inline void		CInput::_sysMouseY ( const int mousey ) {
	Active->sysMouseY = mousey;
}
inline void		CInput::_addRawMouseX ( const int deltamousex ) {
	Active->rawDeltaMouseX += deltamousex;
}
inline void		CInput::_addRawMouseY ( const int deltamousey ) {
	Active->rawDeltaMouseY += deltamousey;
}
inline void		CInput::_currMouseW ( const int mousew ) {
	Active->currMouseW = mousew;
}

inline void		CInput::_mouse ( const int mousebutton_id, bool state ) {
	Active->mouse[mousebutton_id] = state;
}
inline void		CInput::_mousedown ( const int mousebutton_id, bool state ) {
	Active->mousedown[mousebutton_id] = state;
}
inline void		CInput::_mouseup ( const int mousebutton_id, bool state ) {
	Active->mouseup[mousebutton_id] = state;
}

class CKeys
{
public:
	enum KeyEnum
	{
		Back		= 0x08,
		Backspace	= 0x08,
		Tab			= 0x09,
		Return		= 0x0D,
		Escape		= 0x1B,
		Delete		= 0x7F,

		Capital		= 0x0E,
		Capslock	= 0x0E,
		
		Space		= 0x20,
		Shift		= 0x10,
		Control		= 0x11,
		Alt			= 0x12,
		Pause		= 0x13,

		Left		= 0x21,
		Up			= 0x22,
		Right		= 0x23,
		Down		= 0x24,

		F1			= 0x61,
		F2			= 0x62,
		F3			= 0x63,
		F4			= 0x64,
		F5			= 0x65,
		F6			= 0x66,
		F7			= 0x67,
		F8			= 0x68,
		F9			= 0x69,
		F10			= 0x6A,
		F11			= 0x6B,
		F12			= 0x6C
	};

};
extern CKeys Keys;

#endif