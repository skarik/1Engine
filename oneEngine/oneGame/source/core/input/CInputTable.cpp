
#include "CInput.h"
#include "core/os.h"

CKeys Keys;

#ifndef _WIN32

#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
unsigned char _inputtable [256] =
{
};
void CInput::setInputTable ( void )
{
	_inputtable[KEY_F1] = 'a';
	for ( int i = 0; i < 12; ++i ) {
		_inputtable[VK_F1+i] = 'a'+i;
	}
}

#else
//#include "WinUser.h"

unsigned char _inputtable [256] = {};
void CInput::setInputTable ( void )
{
	for ( int i = 0; i < 256; ++i ) {
		_inputtable[i] = 0;
	}
	for ( char a = '0'; a != ':'; ++a ) {
		_inputtable[a] = a;
	}
	for ( char a = 'A'; a != '['; ++a ) {
		_inputtable[a] = a;
	}

	// The following are the actual ASCII mappings
	_inputtable[VK_BACK]			= VK_BACK;
	_inputtable[VK_TAB]				= VK_TAB;
	_inputtable[VK_RETURN]			= VK_RETURN;
	_inputtable[VK_ESCAPE]			= VK_ESCAPE;
	_inputtable[VK_SPACE]			= ' ';	// 32_10, 0x20

	// What follows below are custom mappings from Windows VK to ASCII
	_inputtable[VK_SHIFT]			= VK_SHIFT;		// 0x10
	_inputtable[VK_CONTROL]			= VK_CONTROL;	// 0x11
	_inputtable[VK_MENU]			= VK_MENU;		// 0x12
	_inputtable[VK_PAUSE]			= VK_PAUSE;		// 0x13

	_inputtable[VK_DELETE]			= 0x7F;

	_inputtable[VK_CAPITAL]			= VK_CAPITAL;

	_inputtable[VK_LEFT]			= 0x21;
	_inputtable[VK_UP]				= 0x22;
	_inputtable[VK_RIGHT]			= 0x23;
	_inputtable[VK_DOWN]			= 0x24;

	_inputtable[VK_OEM_3]			= '`';
	_inputtable[VK_OEM_2]			= '/';
	_inputtable[VK_OEM_NEC_EQUAL]	= '=';
	_inputtable[VK_OEM_PERIOD]		= '.';
	_inputtable[VK_OEM_MINUS]		= '-';
	_inputtable[VK_OEM_COMMA]		= ',';
	_inputtable[VK_OEM_1]			= ';';
	_inputtable[VK_OEM_PLUS]		= '+';
	_inputtable[VK_OEM_4]			= '[';
	_inputtable[VK_OEM_5]			= '\\';
	_inputtable[VK_OEM_6]			= ']';
	_inputtable[VK_OEM_7]			= '\'';

	_inputtable[VK_F1]				= 'a';	// Set the function key mappings
	for ( int i = 0; i < 12; ++i ) {
		_inputtable[VK_F1+i] = 'a'+i;
	}
}

#endif