// https://www.khronos.org/opengl/wiki/SPIR-V
// Read our shaders into the appropriate buffers
//std::vector<unsigned char> vertexSpirv = // Get SPIR-V for vertex shader.
//std::vector<unsigned char> fragmentSpirv = // Get SPIR-V for fragment shader.
//
//										   // Create an empty vertex shader handle
//	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//
//// Apply the vertex shader SPIR-V to the shader object.
//glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexSpirv.data(), vertexSpirv.size());
//
//// Specialize the vertex shader.
//std::string vsEntrypoint = ...; // Get VS entry point name
//glSpecializeShader(vertexShader, (const GLchar*)vsEntrypoint.c_str(), 0, nullptr, nullptr);
//
//// Specialization is equivalent to compilation.
//GLint isCompiled = 0;
//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
//if (isCompiled == GL_FALSE)
//{
//	GLint maxLength = 0;
//	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
//
//	// The maxLength includes the NULL character
//	std::vector<GLchar> infoLog(maxLength);
//	glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
//
//	// We don't need the shader anymore.
//	glDeleteShader(vertexShader);
//
//	// Use the infoLog as you see fit.
//
//	// In this simple program, we'll just leave
//	return;
//}
//
//
//
//// Create an empty fragment shader handle
//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//
//// Apply the fragment shader SPIR-V to the shader object.
//glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmentSpirv.data(), fragmentSpirv.size());
//
//// Specialize the fragment shader.
//std::string fsEntrypoint = ...; //Get VS entry point name
//glSpecializeShader(fragmentShader, (const GLchar*)fsEntrypoint.c_str(), 0, nullptr, nullptr);
//
//// Specialization is equivalent to compilation.
//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
//if (isCompiled == GL_FALSE)
//{
//	GLint maxLength = 0;
//	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
//
//	// The maxLength includes the NULL character
//	std::vector<GLchar> infoLog(maxLength);
//	glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
//
//	// We don't need the shader anymore.
//	glDeleteShader(fragmentShader);
//	// Either of them. Don't leak shaders.
//	glDeleteShader(vertexShader);
//
//	// Use the infoLog as you see fit.
//
//	// In this simple program, we'll just leave
//	return;
//}
//
//// Vertex and fragment shaders are successfully compiled.
//// Now time to link them together into a program.
//// Get a program object.
//GLuint program = glCreateProgram();
//
//// Attach our shaders to our program
//glAttachShader(program, vertexShader);
//glAttachShader(program, fragmentShader);
//
//// Link our program
//glLinkProgram(program);
//
//// Note the different functions here: glGetProgram* instead of glGetShader*.
//GLint isLinked = 0;
//glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
//if (isLinked == GL_FALSE)
//{
//	GLint maxLength = 0;
//	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
//
//	// The maxLength includes the NULL character
//	std::vector<GLchar> infoLog(maxLength);
//	glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
//
//	// We don't need the program anymore.
//	glDeleteProgram(program);
//	// Don't leak shaders either.
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//	// Use the infoLog as you see fit.
//
//	// In this simple program, we'll just leave
//	return;
//}
//
//// Always detach shaders after a successful link.
//glDetachShader(program, vertexShader);
//glDetachShader(program, fragmentShader);
#include "RrShaderProgram.h"
#include "RrShaderMasterSubsystem.h"

#include "renderer/gpuw/ShaderPipeline.h"
#include "renderer/gpuw/Shader.h"

#include "core/containers/arstring.h"
#include "core/utils/string.h"
#include "core-ext/resources/ResourceManager.h"

#include "core-ext/system/io/Resources.h"

#include "core/math/hash.h"

#include <inttypes.h>

RrShaderProgram*
RrShaderProgram::Load ( const rrShaderProgramVsPs& params )
{
	auto resm = core::ArResourceManager::Active();

	// Generate the resource name from the filenames:
	arstring256 resource_str_id;
	snprintf(resource_str_id.data, 256, "VsPs_%08" PRIx32 "_%08" PRIx32, math::hash::fnv1a_32(params.file_vv), math::hash::fnv1a_32(params.file_p));
	core::utils::string::ToResourceName(resource_str_id);

	// First, find the texture in the resource system:
	IArResource* existingResource = resm->Find(core::kResourceTypeRrShader, resource_str_id);
	if (existingResource != NULL)
	{
		// Found it! Add a reference and return it.
		RrShaderProgram* existingShader = (RrShaderProgram*)existingShader;
		existingShader->AddReference();
		return existingShader;
	}

	// We need to create a new texture:
	rrStageToLoad stages[] = {
		{gpu::kShaderStageVs, params.file_vv},
		{gpu::kShaderStagePs, params.file_p}
	};
	RrShaderProgram* shaderProgram = LoadShaderProgramFromDisk(resource_str_id.c_str(), stages, 2);

	// Add it to the resource system:
	resm->Add(shaderProgram);

	return shaderProgram;
}

RrShaderProgram*
RrShaderProgram::LoadShaderProgramFromDisk ( const char* s_resourceId, const rrStageToLoad* stages, const int stageCount )
{
	RrShaderProgram* shaderProgram;

	for (int i = 0; i < stageCount; ++i)
	{
		// Generate the actual filenames from the input parameters:
		/*arstring256 filename_vs (params.file_vv);
		core
		// Loop through and try to find the matching filename:
		bool raw_exists = false;
		for (size_t i = 0; i < image_extensions_len; ++i)
		{
		raw_filename = image_rezname + image_extensions[i];
		// Find the file to source data from:
		if (core::Resources::MakePathTo(raw_filename.c_str(), raw_filename))
		{
		raw_exists = true;
		break;
		}
		}*/

		std::string raw_filename = stages[i].filename;

		// Try to find the matching filename:
		bool raw_exists = false;

		// Find the file to source data from:
		if (core::Resources::MakePathTo(raw_filename.c_str(), raw_filename))
		{
			raw_exists = true;
			break;
		}

	}

	/*
	gpu::Shader* vs = new gpu::Shader;
	gpu::Shader* ps = new gpu::Shader;

	vs->createFromFile(gpu::kShaderStageVs, params.file_vv);
	ps->createFromFile(gpu::kShaderStagePs, params.file_p);

	shaderProgram = new RrShaderProgram("", vs, NULL, NULL, NULL, ps, NULL);*/

}


RrShaderProgram::RrShaderProgram (
	const char* s_resourceId,
	gpu::Shader* vs,
	gpu::Shader* hs,
	gpu::Shader* ds,
	gpu::Shader* gs,
	gpu::Shader* ps,
	gpu::Shader* cs
)
{
	if (vs)
		m_pipeline.attach(vs, "main");
	if (hs)
		m_pipeline.attach(hs, "main");
	if (ds)
		m_pipeline.attach(ds, "main");
	if (gs)
		m_pipeline.attach(gs, "main");
	if (ps)
		m_pipeline.attach(ps, "main");
	if (cs)
		m_pipeline.attach(cs, "main");
	m_pipeline.assemble();
}