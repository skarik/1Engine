
#ifndef _C_TIME_PROFILER_UI_H_
#define _C_TIME_PROFILER_UI_H_

#include "core-ext/profiler/CTimeProfiler.h"
#include "renderer/object/CRenderableObject.h"

//#include "glMaterial.h"
//#include "CBitmapFont.h"
class CBitmapFont;

class CTimeProfilerUI : public CRenderableObject
{
	RegisterRenderClassName( "CTimeProfilerUI" );
public:
	// Constructor+Destructor
	CTimeProfilerUI ( void );
	~CTimeProfilerUI ( void );

	// == Stepping Functions ==
	// Render
	bool Render ( const char pass );

private:
	glMaterial*		matFntDebug;
	CBitmapFont*	fntDebug;
};

#endif