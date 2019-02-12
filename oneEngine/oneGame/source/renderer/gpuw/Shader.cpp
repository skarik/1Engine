#include "renderer/gpuw/Shader.h"
#include "renderer/gpuw/Error.h"
#include "renderer/gpuw/Internal/Enums.h"
#include "renderer/ogl/GLCommon.h"

#include <stdio.h>

gpu::Shader::Shader ( void ) :
	m_shaderBytes(0),
	m_shaderLength(0)
{}


int gpu::Shader::createFromFile ( ShaderStage shaderType, const char* file )
{
	char* l_bytes = NULL;
	size_t sz_bytes = 0;

	//todo: replace with fstat needed?

	// Open the file
	FILE* fp = fopen(file, "rb");
	if (fp == NULL)
	{
		return kErrorBadArgument;
	}
	// Go to end to get the file size
	fseek(fp, 0, SEEK_END);
	sz_bytes = ftell(fp);
	// Go to start, allocate data, and read it in
	fseek(fp, 0, SEEK_SET);
	l_bytes = new char[sz_bytes];
	fread(l_bytes, sz_bytes, 1, fp);
	fclose(fp);
	
	// Now use normal shader created with the given data
	//int result = createFromBytes( shaderType, l_bytes, sz_bytes );
	int result;
	{
		m_type = shaderType;

		// Allocate memory to hold the shader.
		destroy();
		m_shaderBytes = l_bytes; //new char[len];
		//memcpy(m_shaderBytes, buffer, len);

		m_shaderLength = sz_bytes;

		// In OpenGL, the shader objects are actually created in the ShaderPipeline::attach.

		result = kError_SUCCESS;
	}

	//delete[] l_bytes; // Free the temp data (since we already copied to GPU-readable memory)
	// Dont free, just use the memory directly.
	return result;
}

int gpu::Shader::createFromBytes ( ShaderStage shaderType, const void* buffer, const size_t len )
{
	m_type = shaderType;

	// Allocate memory to hold the shader.
	destroy();
	m_shaderBytes = new char[len];
	memcpy(m_shaderBytes, buffer, len);

	m_shaderLength = len;

	// In OpenGL, the shader objects are actually created in the ShaderPipeline::attach.

	return kError_SUCCESS;
}

int gpu::Shader::destroy ( void )
{
	if (m_shaderBytes != NULL)
	{
		delete[] m_shaderBytes;
		m_shaderBytes = NULL;
	}
	return kError_SUCCESS;
}