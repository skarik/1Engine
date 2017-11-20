#ifndef RENDERER_SHADER_RENDER_SETTINGS_H_
#define RENDERER_SHADER_RENDER_SETTINGS_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/containers/arstring.h"

namespace renderer
{
	//	rrDeferredShaderSettings : Provides rendering settings.
	struct rrDeferredShaderSettings
	{
		arstring128 filenamePrimaryVertexShader;
		arstring128 filenamePrimaryPixelShader;

		arstring128 filenameStubVertexNormalProcessor;
		arstring128 filenameStubVertexSkinnedProcessor;

		// Default options constructor
		rrDeferredShaderSettings(void) 
		{
			filenamePrimaryVertexShader	= "shaders/def_alt/surface_default.vert";
			filenamePrimaryPixelShader	= "shaders/def_alt/surface_default.frag";

			filenameStubVertexNormalProcessor	= "shaders/def_alt/vertex_proccessing.vert.stub";
			filenameStubVertexSkinnedProcessor	= "shaders/def_alt/vertex_proccessing.skinning.vert.stub";
		}
	};
}

#endif//RENDERER_SHADER_RENDER_SETTINGS_H_