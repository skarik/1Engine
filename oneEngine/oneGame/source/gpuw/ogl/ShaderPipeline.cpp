#include "gpuw/gpuw_common.h"
#ifdef GPU_API_OPENGL

#include "./ShaderPipeline.h"
#include "./Shader.h"
#include "gpuw/Public/Error.h"
#include "./Internal/Enums.h"
#include "./ogl/GLCommon.h"

#include "core/debug/console.h"

gpu::ShaderPipeline::ShaderPipeline ( void )
	:
	m_type(kPipelineTypeInvalid),
	m_shaderVs(NULL), m_shaderHs(NULL), m_shaderDs(NULL), m_shaderGs(NULL), m_shaderPs(NULL),
	m_vs(0), m_hs(0), m_ds(0), m_gs(0), m_ps(0),
	m_program(0)
{}

static int _specializeAndCompileShader ( gpu::ShaderStage shaderType, GLuint shaderHandle, const char* entrypoint_symbol )
{
	// Specialize the shader.
	if (glSpecializeShader)
		glSpecializeShader(shaderHandle, entrypoint_symbol, 0, nullptr, nullptr);
	else
		glSpecializeShaderARB(shaderHandle, entrypoint_symbol, 0, nullptr, nullptr);

	// Specialization is equivalent to compilation.
	GLint isCompiled = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &isCompiled);

	// Check compile state:
	if (isCompiled == GL_FALSE)
	{
		// Grab the output log:
		GLint infoLogLength = 0;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* infoLog = new GLchar [infoLogLength];
		glGetShaderInfoLog(shaderHandle, infoLogLength, &infoLogLength, infoLog);

		// Print out the error log:
		const char* l_shaderReadableId [] = {
			"kShaderStageVs", "kShaderStageHs", "kShaderStageDs", "kShaderStageGs", "kShaderStagePs", "kShaderStageCs"
		};
		debug::Console->PrintError("Compile error in %s with ShaderPipeline. Log follows:\n", l_shaderReadableId[shaderType]);
		debug::Console->PrintError( infoLog );

		delete[] infoLog;

		return gpu::kErrorCreationFailed;
	}
	return gpu::kError_SUCCESS;
}

int gpu::ShaderPipeline::attach ( Shader* shader, const char* entrypoint_symbol )
{
	ARCORE_ASSERT(shader->m_type != kShaderStageCs);
	ARCORE_ASSERT(m_type == kPipelineTypeInvalid || m_type == kPipelineTypeGraphics);

	m_type = kPipelineTypeGraphics;

	// Create the shader
	GLuint shaderHandle = glCreateShader( gpu::internal::ArEnumToGL(shader->m_type) );

	// Apply the shader SPIR-V to the shader object.
	glShaderBinary(1, &shaderHandle, GL_SHADER_BINARY_FORMAT_SPIR_V, shader->m_shaderBytes, (GLsizei)shader->m_shaderLength);

	// Compile the shader:
	if (_specializeAndCompileShader(shader->m_type, shaderHandle, entrypoint_symbol) != kError_SUCCESS)
	{	
		// We don't need the shader anymore.
		glDeleteShader(shaderHandle);
		return gpu::kErrorCreationFailed;
	}

	// Save the shader into our list of shaders:
	switch (shader->m_type)
	{
	case kShaderStageVs:
		m_vs = shaderHandle;
		m_shaderVs = shader;
		break;
	case kShaderStageHs:
		m_hs = shaderHandle;
		m_shaderHs = shader;
		break;
	case kShaderStageDs:
		m_ds = shaderHandle;
		m_shaderDs = shader;
		break;
	case kShaderStageGs:
		m_gs = shaderHandle;
		m_shaderGs = shader;
		break;
	case kShaderStagePs:
		m_ps = shaderHandle;
		m_shaderPs = shader;
		break;
	default:
		// We don't need the invalid shader, actually
		glDeleteShader(shaderHandle);
		return kErrorBadArgument; // Compute shaders are not accepted in a call.
	}

	return kError_SUCCESS;
}

int gpu::ShaderPipeline::attachCompute ( Shader* shader, const char* entrypoint_symbol )
{
	ARCORE_ASSERT(shader->m_type == kShaderStageCs);
	ARCORE_ASSERT(m_type == kPipelineTypeInvalid || m_type == kPipelineTypeCompute);

	m_type = kPipelineTypeCompute;

	// Create the shader
	GLuint shaderHandle = glCreateShader( gpu::internal::ArEnumToGL(shader->m_type) );

	// Apply the shader SPIR-V to the shader object.
	glShaderBinary(1, &shaderHandle, GL_SHADER_BINARY_FORMAT_SPIR_V, shader->m_shaderBytes, (GLsizei)shader->m_shaderLength);

	// Compile the shader:
	if (_specializeAndCompileShader(shader->m_type, shaderHandle, entrypoint_symbol) != kError_SUCCESS)
	{	
		// We don't need the shader anymore.
		glDeleteShader(shaderHandle);
		return gpu::kErrorCreationFailed;
	}

	// Save the shader into our list of shaders:
	switch (shader->m_type)
	{
	case kShaderStageCs:
		m_cs = shaderHandle;
		m_shaderCs = shader;
		break;
	default:
		// We don't need the invalid shader, actually
		glDeleteShader(shaderHandle);
		return kErrorBadArgument; // Only compute shaders accepted.
	}

	return kError_SUCCESS;
}

int gpu::ShaderPipeline::attachRaytracing ( Shader*, const char* )
{
	ARCORE_ASSERT(m_type == kPipelineTypeInvalid || m_type == kPipelineTypeRaytracing);
	ARCORE_ERROR("Not implemented");
	return kErrorModuleUnsupported;
}

int gpu::ShaderPipeline::assemble ( void )
{
	if (m_type == kPipelineTypeInvalid)
		return kErrorCreationFailed;
	if (m_type == kPipelineTypeRaytracing)
		return kErrorModuleUnsupported;

	// Now it's time to link all the compiled shaders!
	// Get a new program object:
	m_program = glCreateProgram();
	
	// Attach our shaders to our program
	if (m_type == kPipelineTypeGraphics)
	{
		if (m_shaderVs != NULL) glAttachShader((GLuint)m_program, (GLuint)m_vs);
		if (m_shaderHs != NULL) glAttachShader((GLuint)m_program, (GLuint)m_hs);
		if (m_shaderDs != NULL) glAttachShader((GLuint)m_program, (GLuint)m_ds);
		if (m_shaderGs != NULL) glAttachShader((GLuint)m_program, (GLuint)m_gs);
		if (m_shaderPs != NULL) glAttachShader((GLuint)m_program, (GLuint)m_ps);
	}
	else if (m_type == kPipelineTypeCompute)
	{
		if (m_shaderCs != NULL) glAttachShader((GLuint)m_program, (GLuint)m_cs);
	}
	
	// Link our program
	glLinkProgram((GLuint)m_program);
	
	// We need to make sure linking was successful.
	GLint isLinked = 0;
	glGetProgramiv((GLuint)m_program, GL_LINK_STATUS, (int *)&isLinked);

	if (isLinked == GL_FALSE)
	{
		// Grab the output log:
		GLint infoLogLength = 0;
		glGetProgramiv((GLuint)m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
	
		GLchar* infoLog = new GLchar [infoLogLength];
		glGetProgramInfoLog((GLuint)m_program, infoLogLength, &infoLogLength, infoLog);

		// Print out the error log:
		debug::Console->PrintError("Linking error in ShaderPipeline:\n");
		debug::Console->PrintError( infoLog );

		delete[] infoLog;
	
		// We don't need the program anymore.
		glDeleteProgram((GLuint)m_program);

		// Don't leak shaders.
		if (m_type == kPipelineTypeGraphics)
		{
			if (m_shaderVs != NULL) glDeleteShader((GLuint)m_vs);
			if (m_shaderHs != NULL) glDeleteShader((GLuint)m_hs);
			if (m_shaderDs != NULL) glDeleteShader((GLuint)m_ds);
			if (m_shaderGs != NULL) glDeleteShader((GLuint)m_gs);
			if (m_shaderPs != NULL) glDeleteShader((GLuint)m_ps);
		}
		else if (m_type == kPipelineTypeCompute)
		{
			if (m_shaderCs != NULL) glDeleteShader((GLuint)m_cs);
		}
	
		return kErrorCreationFailed;
	}

	// Detach the shaders now that we're linked
	if (m_type == kPipelineTypeGraphics)
	{
		if (m_shaderVs != NULL) glDetachShader((GLuint)m_program, (GLuint)m_vs);
		if (m_shaderHs != NULL) glDetachShader((GLuint)m_program, (GLuint)m_hs);
		if (m_shaderDs != NULL) glDetachShader((GLuint)m_program, (GLuint)m_ds);
		if (m_shaderGs != NULL) glDetachShader((GLuint)m_program, (GLuint)m_gs);
		if (m_shaderPs != NULL) glDetachShader((GLuint)m_program, (GLuint)m_ps);
	}
	else if (m_type == kPipelineTypeCompute)
	{
		if (m_shaderCs != NULL) glDetachShader((GLuint)m_program, (GLuint)m_cs);
	}

	// Destroy the shaders now that they're floating unused
	if (m_type == kPipelineTypeGraphics)
	{
		if (m_shaderVs != NULL) glDeleteShader((GLuint)m_vs);
		if (m_shaderHs != NULL) glDeleteShader((GLuint)m_hs);
		if (m_shaderDs != NULL) glDeleteShader((GLuint)m_ds);
		if (m_shaderGs != NULL) glDeleteShader((GLuint)m_gs);
		if (m_shaderPs != NULL) glDeleteShader((GLuint)m_ps);
	}
	else if (m_type == kPipelineTypeCompute)
	{
		if (m_shaderCs != NULL) glDeleteShader((GLuint)m_cs);
	}

	return kError_SUCCESS;
}

int gpu::ShaderPipeline::destroy ( void )
{
	glDeleteProgram((GLuint)m_program);
	m_program = 0;
	return kError_SUCCESS;
}

bool gpu::ShaderPipeline::valid ( void )
{
	return m_program != 0;
}

#endif