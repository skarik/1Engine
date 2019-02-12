#include "renderer-console.h"

#include "core-ext/resources/ResourceManager.h"
#include "renderer/material/RrShaderMasterSubsystem.h"
#include "renderer/material/RrShaderProgram.h"

int renderer::con_recompile_shader ( std::string const& cmd )
{
	/*RrShader* shader = ShaderManager.ShaderExists( cmd, renderer::SHADER_TAG_DEFAULT );
	if ( shader != NULL ) {
		shader->recompile();
	}*/
	auto resm = core::ArResourceManager::Active();
	IArResource* existingResource = resm->Find(core::kResourceTypeRrShader, cmd.c_str());
	if (existingResource != NULL)
	{
		RrShaderProgram* existingShader = (RrShaderProgram*)existingResource;
		existingShader->AddReference();
		ARCORE_ERROR("Shader recompiling is not supported at this time.\n");
		existingShader->RemoveReference();
		return 0;
	}
	else
	{
		return 1;
	}
}
int renderer::con_recompile_shaders ( std::string const& cmd )
{
	auto resm = core::ArResourceManager::Active();
	resm->GetSubsystem(core::kResourceTypeRrShader)->Reload();
	
	return 0;
}