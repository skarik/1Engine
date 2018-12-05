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
#include "renderer/gpuw/Error.h"

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
		RrShaderProgram* existingShader = (RrShaderProgram*)existingResource;
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
	gpu::Shader* shaderStages [8] = {};

	// Loop through all the input stages and load them one-by-one.
	// We will save each stage into the array "shaderStages[]" then pass all into the RrShaderProgram constructor.
	for (int i = 0; i < stageCount; ++i)
	{
		std::string sb_filename = stages[i].filename;

		// Find the file to source data from:
		bool file_found = core::Resources::MakePathTo(sb_filename.c_str(), sb_filename);
		ARCORE_ASSERT(file_found);

		if (file_found)
		{
			// Create and load the shader from disk.
			gpu::Shader* shader = new gpu::Shader;
			int ret = shader->createFromFile(stages[i].stage, sb_filename.c_str());
			ARCORE_ASSERT(ret ==  gpu::kError_SUCCESS);

			// Save the shader for now.
			shaderStages[stages[i].stage] = shader;
		}
	}

	// Create the shader program
	RrShaderProgram* shaderProgram;
	shaderProgram = new RrShaderProgram(s_resourceId,
		shaderStages[gpu::kShaderStageVs],
		shaderStages[gpu::kShaderStageHs],
		shaderStages[gpu::kShaderStageDs],
		shaderStages[gpu::kShaderStageGs],
		shaderStages[gpu::kShaderStagePs],
		shaderStages[gpu::kShaderStageCs]
	);

	return shaderProgram;
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
	// Assign resource name
	m_resourceName = s_resourceId;

	// Attach all valid inputs to the shader
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

	// Assemble the shader pipeline now that everything has been added to it.
	int ret = m_pipeline.assemble();

	ARCORE_ASSERT(ret == gpu::kError_SUCCESS);
	ARCORE_ASSERT(m_pipeline.valid());
}