#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./ShaderPipeline.dx11.h"
#include "./Shader.dx11.h"
#include "./Device.dx11.h"
#include "gpuw/Public/Error.h"

#include "core/debug.h"
#include "core/debug/console.h"
#include "core/exceptions.h"

gpu::ShaderPipeline::ShaderPipeline ( void )
	:
	m_type(kPipelineTypeInvalid),
	m_shaderVs(NULL), m_shaderHs(NULL), m_shaderDs(NULL), m_shaderGs(NULL), m_shaderPs(NULL),
	m_vs(0), m_hs(0), m_ds(0), m_gs(0), m_ps(0),
	m_program(0)
{}

int gpu::ShaderPipeline::attach ( Shader* shader, const char* entrypoint_symbol )
{
	ID3D11Device* device = gpu::getDevice()->getNative();
	HRESULT			result;

	ARCORE_ASSERT(shader->m_type != kShaderStageCs);
	ARCORE_ASSERT(m_type == kPipelineTypeInvalid || m_type == kPipelineTypeGraphics);

	m_type = kPipelineTypeGraphics;

	// Save the shader into our list of shaders:
	switch (shader->m_type)
	{
	case kShaderStageVs:
		result = device->CreateVertexShader( shader->m_shaderBytes, shader->m_shaderLength, NULL, (ID3D11VertexShader**)&m_vs );
		m_shaderVs = shader;
		break;
	case kShaderStageHs:
		result = device->CreateHullShader( shader->m_shaderBytes, shader->m_shaderLength, NULL, (ID3D11HullShader**)&m_hs );
		m_shaderHs = shader;
		break;
	case kShaderStageDs:
		result = device->CreateDomainShader( shader->m_shaderBytes, shader->m_shaderLength, NULL, (ID3D11DomainShader**)&m_ds );
		m_shaderDs = shader;
		break;
	case kShaderStageGs:
		result = device->CreateGeometryShader( shader->m_shaderBytes, shader->m_shaderLength, NULL, (ID3D11GeometryShader**)&m_gs );
		m_shaderGs = shader;
		break;
	case kShaderStagePs:
		result = device->CreatePixelShader( shader->m_shaderBytes, shader->m_shaderLength, NULL, (ID3D11PixelShader**)&m_ps );
		m_shaderPs = shader;
		break;
	default:
		return kErrorBadArgument; // Compute shaders are not accepted in a call.
	}

	if (FAILED(result))
	{
		throw core::CorruptedDataException();
		return kErrorCreationFailed;
	}

	return kError_SUCCESS;
}

int gpu::ShaderPipeline::attachCompute ( Shader* shader, const char* entrypoint_symbol )
{
	ID3D11Device* device = gpu::getDevice()->getNative();
	HRESULT			result;

	ARCORE_ASSERT(shader->m_type == kShaderStageCs);
	ARCORE_ASSERT(m_type == kPipelineTypeInvalid || m_type == kPipelineTypeCompute);

	m_type = kPipelineTypeCompute;

	// Save the shader into our list of shaders:
	switch (shader->m_type)
	{
	case kShaderStageCs:
		result = device->CreateComputeShader( shader->m_shaderBytes, shader->m_shaderLength, NULL, (ID3D11ComputeShader**)&m_cs );
		m_shaderCs = shader;
		break;
	default:
		return kErrorBadArgument; // Only compute shaders accepted.
	}

	if (FAILED(result))
	{
		throw core::CorruptedDataException();
		return kErrorCreationFailed;
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
	/*8m_program = glCreateProgram();
	
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
	}*/

	return kError_SUCCESS;
}

int gpu::ShaderPipeline::destroy ( void )
{
	if (m_type == kPipelineTypeGraphics)
	{
		if (m_shaderVs != NULL) static_cast<ID3D11DeviceChild*>((void*)m_vs)->Release();
		if (m_shaderHs != NULL) static_cast<ID3D11DeviceChild*>((void*)m_hs)->Release();
		if (m_shaderDs != NULL) static_cast<ID3D11DeviceChild*>((void*)m_vs)->Release();
		if (m_shaderGs != NULL) static_cast<ID3D11DeviceChild*>((void*)m_vs)->Release();
		if (m_shaderPs != NULL) static_cast<ID3D11DeviceChild*>((void*)m_vs)->Release();
	}
	else
	{
		if (m_shaderCs != NULL) static_cast<ID3D11DeviceChild*>((void*)m_cs)->Release();
	}
	return kError_SUCCESS;
}

bool gpu::ShaderPipeline::valid ( void )
{
	if (m_type == kPipelineTypeGraphics)
	{
		return true;
	}
	else if (m_type == kPipelineTypeCompute)
	{
		return true;
	}

	return false;
}

#endif