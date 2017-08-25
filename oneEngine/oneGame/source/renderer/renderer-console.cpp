
#include "renderer-console.h"

#include "renderer/material/RrShaderManager.h"

int renderer::con_recompile_shader ( std::string const& cmd )
{
	RrShader* shader = ShaderManager.ShaderExists( cmd, renderer::SHADER_TAG_DEFAULT );
	if ( shader != NULL ) {
		shader->recompile();
	}
	return 0;
}
int renderer::con_recompile_shaders ( std::string const& cmd )
{
	ShaderManager.RecompileAll();
	return 0;
}