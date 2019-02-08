#ifndef ENGINE_COMMON_UTIL_DEVELOPER_CONSOLE_UI_H_
#define ENGINE_COMMON_UTIL_DEVELOPER_CONSOLE_UI_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

#include "renderer/object/CRenderableObject.h"

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

	bool			PreRender ( rrCameraPass* cameraPass ) override;
	bool			Render ( const rrRenderParams* params ) override;

private:
	RrFontTexture*	fntMenu;
};

class CDeveloperCursor : public CRenderableObject
{
	ClassName( "CDeveloperCursor" );
public:
	ENGCOM_API		CDeveloperCursor ( void );
	ENGCOM_API		~CDeveloperCursor ( void );

	bool			PreRender ( rrCameraPass* cameraPass ) override;
	bool			Render ( const rrRenderParams* params ) override;;

private:
	RrTexture*		texCursor;
};

ENGCOM_API extern CDeveloperConsoleUI*	ActiveConsoleUI;
ENGCOM_API extern CDeveloperCursor*	ActiveCursor;

#endif//ENGINE_COMMON_UTIL_DEVELOPER_CONSOLE_UI_H_