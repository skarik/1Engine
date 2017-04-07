
#ifndef _DEPLOY_UNIT_H_
#define _DEPLOY_UNIT_H_

#include "core/types/types.h"

#ifdef _WIN32
#	define ARUNIT_ARGS HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow
#	define ARUNIT_CALL __stdcall
#	define ARUNIT_BUILD_CMDLINE
#	define ARUNIT_MAIN WinMain
#else
#	define ARUNIT_ARGS int argc, char **argv
#	define ARUNIT_CALL
#	define ARUNIT_BUILD_CMDLINE char* lpCmdLine = NULL;
#	define ARUNIT_MAIN main
#endif

namespace Unit
{
	DEPLOY_API int ARUNIT_CALL Test_EngineCore ( ARUNIT_ARGS );
	DEPLOY_API int ARUNIT_CALL Test_Core ( ARUNIT_ARGS );
	DEPLOY_API int ARUNIT_CALL Test_Audio ( ARUNIT_ARGS );
	DEPLOY_API int ARUNIT_CALL Test_EngineCommon ( ARUNIT_ARGS );
};

#endif//_DEPLOY_UNIT_H_