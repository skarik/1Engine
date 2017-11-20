#ifndef DEPLOY_GAME_H_
#define DEPLOY_GAME_H_

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

namespace Deploy
{
	DEPLOY_API int _ARUNIT_CALL Game ( _ARUNIT_ARGS );
};

#endif//DEPLOY_GAME_H_