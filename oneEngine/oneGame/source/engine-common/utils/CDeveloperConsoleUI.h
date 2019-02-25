#ifndef ENGINE_COMMON_UTIL_DEVELOPER_CONSOLE_UI_H_
#define ENGINE_COMMON_UTIL_DEVELOPER_CONSOLE_UI_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

#include "renderer/object/CRenderableObject.h"
#include "renderer/object/mesh/system/rrMeshBuffer.h"
#include "renderer/object/mesh/system/rrMeshBufferRing.h"

class RrFontTexture;
class RrTexture;

using std::string;

class CDeveloperConsoleUI;
class CDeveloperCursor;
class CTimeProfilerUI;

class CDeveloperConsoleUI : public CRenderableObject
{
	RegisterRenderClassName( "CDeveloperConsoleUI" );
public:
	ENGCOM_API				CDeveloperConsoleUI ( void );
	ENGCOM_API				~CDeveloperConsoleUI ( void );

	bool					BeginRender ( void ) override;
	bool					PreRender ( rrCameraPass* cameraPass ) override;
	bool					Render ( const rrRenderParams* params ) override;

private:
	RrFontTexture*		fntMenu;

	rrMeshBufferRing<3>	m_meshBufferShapes;
	rrMeshBufferRing<3>	m_meshBufferText;
	uint16_t			m_indexCountShapes;
	uint16_t			m_indexCountText;
};

class CDeveloperCursor : public CRenderableObject
{
	RegisterRenderClassName( "CDeveloperCursor" );
public:
	ENGCOM_API				CDeveloperCursor ( void );
	ENGCOM_API				~CDeveloperCursor ( void );

	bool					PreRender ( rrCameraPass* cameraPass ) override;
	bool					Render ( const rrRenderParams* params ) override;;

private:
	RrTexture*			texCursor;

	rrMeshBuffer		m_meshBuffer;
	uint16_t			m_indexCount;
};

ENGCOM_API extern CDeveloperConsoleUI*	ActiveConsoleUI;
ENGCOM_API extern CDeveloperCursor*	ActiveCursor;

#endif//ENGINE_COMMON_UTIL_DEVELOPER_CONSOLE_UI_H_