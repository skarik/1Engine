#include "renderer/gpuw/Shader.h"
#include "renderer/gpuw/Error.h"

#include <stdio.h>

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
	int result = createFromBytes( shaderType, l_bytes, sz_bytes );
	delete[] l_bytes; // Free the temp data (since we already copied to GPU-readable memory)
	return result;
}