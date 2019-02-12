#ifndef GL_COMMON_H_
#define GL_COMMON_H_
//#define GL_GLEXT_PROTOTYPES 1 
#	ifdef _WIN32
#		include "core/os.h"
#		include "glcorearb.h"
#		include "glcorearb_impl.h"
#		include "wgl.h"
#		include "wgl_impl.h"
#elif __linux
#		include "core/os.h"
#		include "glcorearb.h"
#		include "glxext.h"
#endif
#endif//GL_COMMON_H_