
#include "renderer-console.h"

#include "renderer/material/glShaderManager.h"

int Renderer::con_recompile_shader ( std::string const& cmd )
{
	glShader* shader = ShaderManager.ShaderExists( cmd, GLE::SHADER_TAG_DEFAULT );
	if ( shader != NULL ) {
		shader->recompile();
	}
	return 0;
}
int Renderer::con_recompile_shaders ( std::string const& cmd )
{
	ShaderManager.RecompileAll();
	return 0;
}