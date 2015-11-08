
#ifndef _RENDERER_CONSOLE_H_
#define _RENDERER_CONSOLE_H_

//===============================================================================================//
// Renderer-console-console
// 
// renderer functionality specific to the console
// Since the renderer cannot see the console (console is in engine) these are added to the console
// within engine-common.cpp upon EngineCommonInitialize().
//===============================================================================================//

#include <string>

#include "core/types/types.h"

namespace Renderer
{
	RENDER_API int con_recompile_shader ( std::string const& cmd );
	RENDER_API int con_recompile_shaders ( std::string const& cmd );
};

#endif//_RENDERER_CONSOLE_H_