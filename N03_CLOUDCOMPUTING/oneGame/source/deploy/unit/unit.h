
#ifndef _DEPLOY_UNIT_H_
#define _DEPLOY_UNIT_H_

#include "core/os.h"
#include "core/types/types.h"

#ifdef _WIN32
#	define _ARUNIT_ARGS HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow
#	define _ARUNIT_CALL __stdcall
#	define _ARUNIT_BUILD_CMDLINE
#else
#	define _ARUNIT_ARGS int argc, char **argv
#	define _ARUNIT_CALL
#	define _ARUNIT_BUILD_CMDLINE char* lpCmdLine = NULL;
#endif

namespace Unit
{
	DEPLOY_API int _ARUNIT_CALL Test_EngineCore ( _ARUNIT_ARGS );
	DEPLOY_API int _ARUNIT_CALL Test_Core ( _ARUNIT_ARGS );
	DEPLOY_API int _ARUNIT_CALL Test_Audio ( _ARUNIT_ARGS );
	DEPLOY_API int _ARUNIT_CALL Test_EngineCommon ( _ARUNIT_ARGS );
};

#endif//_DEPLOY_UNIT_H_