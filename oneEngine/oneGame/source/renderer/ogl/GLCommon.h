
#ifndef __GL_COMMON_FUNCTION_H__
#define __GL_COMMON_FUNCTION_H__

//HMMM THIS LOOKS NIFTY GET IT WORKING LATER
//#define GL_GLEXT_PROTOTYPES 1 

// Needs the OpenGL functions
#ifdef _WIN32
#	include "core/os.h"

	//#include "gl_core_2_1.h"
	//#include "gl_core_3_0.h"
	//#include "gl_compat_3_1.h"	// OpenGL 3.1 spec
	//#include "wgl_core.h"		// OpenGL Win32 extensions
	#include "gl_core_3_3.h"
	#include "wgl_core_3_3.h"

	#include <gl\glu.h>			// Header File For The GLu32 Library
#elif __linux

#endif

#endif