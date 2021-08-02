#ifndef ENGINE_COMMON_TIME_PROFILER_UI_H_
#define ENGINE_COMMON_TIME_PROFILER_UI_H_

#include "core-ext/profiler/CTimeProfiler.h"
#include "renderer/object/RrRenderObject.h"

//#include "RrMaterial.h"
//#include "RrFontTexture.h"
class RrFontTexture;

class CTimeProfilerUI : public RrRenderObject
{
	RegisterRenderClassName( "CTimeProfilerUI" );
public:
	ENGCOM_API				CTimeProfilerUI ( void );
	ENGCOM_API				~CTimeProfilerUI ( void );

	bool					Render ( const rrRenderParams* params ) override;

private:
	//RrMaterial*		matFntDebug;
	RrFontTexture*	fntDebug;
};

#endif//ENGINE_COMMON_TIME_PROFILER_UI_H_