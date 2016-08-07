// Includes
#include "CInput.h"
#include "CXboxController.h"

#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"

// Static variable declaration
/*bool	CInput::key[256];
bool	CInput::keyup[256];
bool	CInput::keydown[256];
float	CInput::mouseX = 200;
float	CInput::mouseY = 200;
int		CInput::sysMouseX = 0;
int		CInput::sysMouseY = 0;
//float	CInput::prevMouseX = 0;
//float	CInput::prevMouseY = 0;
float	CInput::deltaMouseX = 0;
float	CInput::deltaMouseY = 0;
int		CInput::rawDeltaMouseX = 0;
int		CInput::rawDeltaMouseY = 0;
int		CInput::prevRawDeltaMouseX = 0;
int		CInput::prevRawDeltaMouseY = 0;
int		CInput::currMouseW = 0;
int		CInput::deltaMouseW = 0;
bool	CInput::mouse[4];
bool	CInput::mousedown[4];
bool	CInput::mouseup[4];
int		CInput::prevMouseW = 0;*/

InputControl::CXBoxController* CInput::xboxControl = NULL;
CInput* CInput::Active = NULL;

void CInput::Initialize ( void )
{
	if ( Active == NULL ) {
		Active = new CInput();
	}

	Active->setInputTable();
	
	// Zero input
	for ( int i = 0; i < 256; ++i )
	{
		Active->key[i]		= false;
		Active->keyup[i]	= false;
		Active->keydown[i]	= false;
	}
	for ( int i = 0; i < 4; ++i )
	{
		Active->mouse[i]	= false;
		Active->mousedown[i]= false;
		Active->mouseup[i]	= false;
	}
	SetMouseSensitivity( 1.0F );

	xboxControl = new InputControl::CXBoxController(1);
}
void CInput::Reset ( void )
{
	Free();
	Initialize();
}
void CInput::Free ( void )
{
	if ( Active ) {
		delete Active;
		Active = NULL;
	}
	delete xboxControl;
	xboxControl = NULL;
}


void CInput::PreUpdate ( void )
{
	//prevRawDeltaMouseX = 0;
		Active->rawDeltaMouseX = 0;
	//	deltaMouseX = 0.0f;

	//prevRawDeltaMouseY = 0;
		Active->rawDeltaMouseY = 0;
	//	deltaMouseY = 0.0f;
}

// Static function declare
void CInput::Update ( void )
{
	Active->_Update();
}
void CInput::_Update ( void )
{
	for ( int i = 0; i < 256; i += 1 )
	{
		if ( keydown[i] )
		{
			keydown[i] = false;
		}
		if ( keyup[i] )
		{
			keyup[i] = false;
		}
	}

	for ( int i = 0; i < 4; i += 1 )
	{
		if ( mousedown[i] )
		{
			mousedown[i] = false;
		}
		if ( mouseup[i] )
		{
			mouseup[i] = false;
		}
	}

	//deltaMouseX = mouseX-prevMouseX;
	//deltaMouseY = mouseY-prevMouseY;
	//prevMouseX = mouseX;
	//prevMouseY = mouseY;

	//if ( prevRawDeltaMouseX != rawDeltaMouseX )
	{
		prevRawDeltaMouseX = rawDeltaMouseX;
		deltaMouseX = rawDeltaMouseX * CGameSettings::Active()->f_cl_MouseSensitivity;
	}
	/*else
	{
		prevRawDeltaMouseX = 0;
		rawDeltaMouseX = 0;
		deltaMouseX = 0.0f;
	}*/
	//if ( prevRawDeltaMouseY != rawDeltaMouseY )
	{
		prevRawDeltaMouseY = rawDeltaMouseY;
		deltaMouseY = rawDeltaMouseY * CGameSettings::Active()->f_cl_MouseSensitivity;
	}
	/*else
	{
		prevRawDeltaMouseY = 0;
		rawDeltaMouseY = 0;
		deltaMouseY = 0.0f;
	}*/
	
	// offset them real mouse posses, limiting to window
	mouseX += deltaMouseX * mouseSensitivityX;
	mouseX = (mouseX<0) ? 0 : mouseX;
	mouseX = (mouseX>Screen::Info.width) ? Screen::Info.width : mouseX;
	mouseY += deltaMouseY * mouseSensitivityY;
	mouseY = (mouseY<0) ? 0 : mouseY;
	mouseY = (mouseY>Screen::Info.height) ? Screen::Info.height : mouseY;

	if ( currMouseW != prevMouseW )
	{
		deltaMouseW = currMouseW;
		prevMouseW = currMouseW;
	}
	else
	{
		deltaMouseW = 0;
		currMouseW = 0;
		prevMouseW = 0;
	}
}

void CInput::SetMouseSensitivity ( const Real sensitivity )
{
	Active->mouseSensitivityX = sensitivity;
	Active->mouseSensitivityY = sensitivity;
}
Real CInput::GetMouseSensitivity ( void )
{
	return Active->mouseSensitivityX;
}


bool CInput::Keypress ( unsigned char const keycode_ascii )
{
	return Active->key[keycode_ascii];
}
bool CInput::Key ( unsigned char const keycode_ascii )
{
	return Active->key[keycode_ascii];
}
bool CInput::Keydown ( unsigned char const keycode_ascii )
{
	return Active->keydown[keycode_ascii];
}
bool CInput::Keyup ( unsigned char const keycode_ascii )
{
	return Active->keyup[keycode_ascii];
}

bool CInput::KeypressAny ( void )
{
	for ( unsigned short i = 0; i < 256; ++i ) {
		if ( Active->key[i] ) {
			return true;
		}
	}
	return false;
}
bool CInput::KeydownAny ( void )
{
	for ( unsigned short i = 0; i < 256; ++i ) {
		if ( Active->keydown[i] ) {
			return true;
		}
	}
	return false;
}
bool CInput::KeyupAny ( void )
{
	for ( unsigned short i = 0; i < 256; ++i ) {
		if ( Active->keyup[i] ) {
			return true;
		}
	}
	return false;
}

unsigned char CInput::GetTypeChar ( void )
{
	/*for ( unsigned char i = 'A'; i <= 'Z'; ++i )
	{
		
	}*/
	/// First get the key pressed
	unsigned char kc = 0;
	for ( unsigned short i = 0; i < 256; ++i ) {
		if (( i == Keys.Control )||( i == Keys.Shift )) {
			continue;
		}
		if ( Active->keydown[i] ) {
			kc = (unsigned char)i;
			break;
		}
	}

	if ( kc == Keys.Space ) {
		return kc;
	}
	
	if ( Active->key[Keys.Shift] )
	{
		if ( kc >= 'A' && kc <= 'Z' ) {
			return kc;
		}
		switch ( kc )
		{
			case '1': return '!';
			case '2': return '@';
			case '3': return '#';
			case '4': return '$';
			case '5': return '%';
			case '6': return '^';
			case '7': return '&';
			case '8': return '*';
			case '9': return '(';
			case '0': return ')';

			case '[': return '{';
			case ']': return '}';
			case '\\': return '|';
			case ';': return ':';
			case '\'': return '"';
			case ',': return '<';
			case '.': return '>';
			case '/': return '?';

			case '-': return '_';
			case '=':
			case '+': return '+';

			default: break;
		}
	}
	else
	{
		if ( kc >= 'A' && kc <= 'Z' ) {
			return kc-'A'+'a';
		}
		if ( kc >= '0' && kc <= '9' ) {
			return kc;
		}
		switch ( kc )
		{
		case '[':
		case ']':
		case '\\':
		case ';':
		case '\'':
		case ',':
		case '.':
		case '/':
		case '-':
			return kc;
		case '=':
		case '+':
			return '=';
		default:
			break;
		}
	}

	return 0;
}


float CInput::MouseX ( void ) {
	return Active->mouseX;
}
float CInput::MouseY ( void ) {
	return Active->mouseY;
}

int CInput::SysMouseX ( void ) {
	return Active->sysMouseX;
}
int CInput::SysMouseY ( void ) {
	return Active->sysMouseY;
}

float CInput::DeltaMouseX ( void ) {
	return Active->deltaMouseX;
}
float CInput::DeltaMouseY ( void ) {
	return Active->deltaMouseY;
}
int CInput::DeltaMouseW ( void ) {
	return Active->deltaMouseW;
}

bool CInput::Mouse ( const int mousebutton_id ) {
	return Active->mouse[mousebutton_id];
}
bool CInput::MouseDown ( const int mousebutton_id ) {
	return Active->mousedown[mousebutton_id];
}
bool CInput::MouseUp ( const int mousebutton_id ) {
	return Active->mouseup[mousebutton_id];
}