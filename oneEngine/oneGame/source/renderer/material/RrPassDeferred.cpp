#include "RrShader.h"
#include "RrPassDeferred.h"
#include "renderer/types/ObjectSettings.h"
#include <algorithm>

namespace renderer
{
	extern rrDeferredShaderSettings gDefferedShaderSettings = rrDeferredShaderSettings();
}

RrPassDeferred::RrPassDeferred ( void )
	: m_dirty(true),
	/*m_blend_mode(renderer::BM_NONE),*/ m_transparency_mode(renderer::ALPHAMODE_NONE),
	/*m_lighting_mode(renderer::LI_NORMAL), m_diffuse_method(renderer::Deferred::DIFFUSE_DEFAULT),*/
	m_rimlight_strength(0.2f),
	m_ready(false), shader(NULL)
{

}
RrPassDeferred::~RrPassDeferred( void )
{
	if ( shader )
	{
		shader->ReleaseReference();
	}
}

// Need to do a lot of file building.
/*void RrPassDeferred::buildShader ( void )
{

}*/

#include "RrMaterial.h"
#include "core/system/IO/FileUtils.h"
#include "core-ext/system/io/Resources.h"
#include <cctype>
#include <map>

void RrMaterial::deferred_shader_build( uchar pass )
{
	RrPassDeferred& dpass = deferredinfo[pass];
	// Build a shader based on the dpass.

	std::string				t_shaderName;
	renderer::rrShaderTag	t_shaderTag;

	// Generate shader name based on settings
	t_shaderName = ".game/.dfshaders/shader";
	t_shaderName += ".am" + std::to_string((uint)dpass.m_transparency_mode);
	t_shaderName += m_isSkinnedShader ? ".skinning" : "";

	// there are 6 variations:
	int variation = (int)dpass.m_transparency_mode + 3 * (m_isSkinnedShader?1:0);

	// Change shader tags
	t_shaderTag = renderer::SHADER_TAG_DEFAULT;
	if ( m_isSkinnedShader )
	{
		t_shaderTag = renderer::SHADER_TAG_SKINNING;
	}

	// ============================
	// Build the vertex shader
	{
		std::string shader;
		size_t pos;

		std::string t_fnPrimaryShader = renderer::gDefferedShaderSettings.filenamePrimaryVertexShader;
		if (IO::FileExists( core::Resources::GetPrimaryResourcePath() + t_fnPrimaryShader ))
		{
			shader = IO::ReadFileToString( (core::Resources::GetPrimaryResourcePath() + t_fnPrimaryShader).c_str() );
		}
		else
		{
			shader = IO::ReadFileToString( core::Resources::PathTo(t_fnPrimaryShader).c_str() );
		}

		// Remove macro at the top
		pos = shader.find("#define VERTEX_PROCESSOR");
		if (pos != std::string::npos)
		{
			shader.erase(pos, sizeof("#define VERTEX_PROCESSOR"));
		}

		// Add macro at the bottom
		pos = shader.find("VERTEX_PROCESSOR");
		if (pos != std::string::npos)
		{
			shader.erase(pos, sizeof("VERTEX_PROCESSOR"));
			std::string stub;
			if ( t_shaderTag == renderer::SHADER_TAG_SKINNING )
				stub = IO::ReadFileToString( core::Resources::PathTo(renderer::gDefferedShaderSettings.filenameStubVertexSkinnedProcessor).c_str() );
			else if ( t_shaderTag == renderer::SHADER_TAG_DEFAULT )
				stub = IO::ReadFileToString( core::Resources::PathTo(renderer::gDefferedShaderSettings.filenameStubVertexNormalProcessor).c_str() );
			shader.insert(pos, stub);
		}

		std::string t_shaderNameVert = t_shaderName + ".vert";
		IO::ClearFile( t_shaderNameVert.c_str() );
		IO::AppendStringToFile( t_shaderNameVert.c_str(), shader.c_str() );
	}

	// ============================
	// Build the pixel shader
	{
		std::string shader;

		std::string t_fnPrimaryShader = renderer::gDefferedShaderSettings.filenamePrimaryPixelShader;
		if (IO::FileExists( core::Resources::GetPrimaryResourcePath() + t_fnPrimaryShader ))
		{
			shader = IO::ReadFileToString( (core::Resources::GetPrimaryResourcePath() + t_fnPrimaryShader).c_str() );
		}
		else
		{
			shader = IO::ReadFileToString( core::Resources::PathTo(t_fnPrimaryShader).c_str() );
		}

		std::string t_shaderNameFrag = t_shaderName+".frag";
		IO::ClearFile( t_shaderNameFrag.c_str() );
		IO::AppendStringToFile( t_shaderNameFrag.c_str(), shader.c_str() );
	}

	// Create the shader
	dpass.shader = new RrShader( (t_shaderName+".glsl").c_str(), t_shaderTag );
}
