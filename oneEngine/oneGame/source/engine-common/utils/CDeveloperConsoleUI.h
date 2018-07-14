
#ifndef _C_DEVELOPER_CONSOLE_UI_H_
#define _C_DEVELOPER_CONSOLE_UI_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

#include "renderer/object/CRenderableObject.h"
//#include "rendererRrFontTexture.h"
class RrFontTexture;
class RrTexture;

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

	bool			PreRender ( void ) override;
	bool			Render ( const char pass ) override;

private:
	RrFontTexture*	fntMenu;
	RrMaterial*		matfntMenu;
	RrMaterial*		matMenu;
};

class CDeveloperCursor : public CRenderableObject
{
	ClassName( "CDeveloperCursor" );
public:
	ENGCOM_API		CDeveloperCursor ( void );
	ENGCOM_API		~CDeveloperCursor ( void );

	bool			PreRender ( void ) override;
	bool			Render ( const char pass ) override;;

private:
	RrMaterial*		matCursor;
	RrTexture*		texCursor;
};

ENGCOM_API extern CDeveloperConsoleUI*	ActiveConsoleUI;
ENGCOM_API extern CDeveloperCursor*	ActiveCursor;

#endif//_C_DEVELOPER_CONSOLE_UI_H_