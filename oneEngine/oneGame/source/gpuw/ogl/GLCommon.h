#ifndef GL_COMMON_H_
#define GL_COMMON_H_

//===============================================================================================//
// Windows
#	ifdef _WIN32
#		include "core/os.h"
// Load up OpenGL lib in the header as we use OpenGL
#		pragma comment(lib, "OpenGL32.lib")
// Load the OpenGL headers
#		include "glcorearb.h"
#		include "glcorearb_impl.h"
#		include "wgl.h"
#		include "wgl_impl.h"

//===============================================================================================//
// Linux
#	elif __linux
#		pragma comment(lib, "OpenGL32.a")
#		include "core/os.h"
#		include "glcorearb.h"
#		include "glxext.h"

#	endif

#endif//GL_COMMON_H_