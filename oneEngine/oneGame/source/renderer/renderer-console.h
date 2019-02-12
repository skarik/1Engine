//===============================================================================================//
//
// Renderer-console
// 
// renderer functionality specific to the console
// Since the renderer cannot see the console (console is in engine) these are added to the console
// within engine-common.cpp upon EngineCommonInitialize().
//
//===============================================================================================//
#ifndef RENDERER_CONSOLE_H_
#define RENDERER_CONSOLE_H_

#include <string>
#include "core/types/types.h"

namespace renderer
{
	RENDER_API int con_recompile_shader ( std::string const& cmd );
	RENDER_API int con_recompile_shaders ( std::string const& cmd );
};

#endif//RENDERER_CONSOLE_H_