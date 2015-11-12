
#ifndef _C_DEVELOPER_CONSOLE_UI_H_
#define _C_DEVELOPER_CONSOLE_UI_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

#include "renderer/object/CRenderableObject.h"
//#include "rendererCBitmapFont.h"
class CBitmapFont;
class CTexture;

using std::string;

class CDeveloperConsoleUI;
class CDeveloperCursor;
class CTimeProfilerUI;

class CDeveloperConsoleUI : public CRenderableObject
{
	ClassName( "CDeveloperConsoleUI" );
public:
	ENGCOM_API		CDeveloperConsoleUI ( void );
	ENGCOM_API		~CDeveloperConsoleUI ( void );

	bool			Render ( const char pass );

private:
	CBitmapFont*	fntMenu;
	glMaterial*		matfntMenu;
	glMaterial*		matMenu;
};

class CDeveloperCursor : public CRenderableObject
{
	ClassName( "CDeveloperCursor" );
public:
	ENGCOM_API		CDeveloperCursor ( void );
	ENGCOM_API		~CDeveloperCursor ( void );

	bool			Render ( const char pass );

private:
	glMaterial*		matCursor;
	CTexture*		texCursor;
};

ENGCOM_API extern CDeveloperConsoleUI*	ActiveConsoleUI;
ENGCOM_API extern CDeveloperCursor*	ActiveCursor;

#endif//_C_DEVELOPER_CONSOLE_UI_H_