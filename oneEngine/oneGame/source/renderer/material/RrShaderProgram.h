#ifndef RENDERER_SHADER_PROGRAM_H_
#define RENDERER_SHADER_PROGRAM_H_

#include "core/types/arBaseObject.h"
#include "core-ext/resources/IArResource.h"
#include "renderer/gpuw/ShaderPipeline.h"

struct rrShaderProgramVsPs
{
	const char*			file_vv;
	const char*			file_p;
};

struct rrShaderProgramVsGsPs
{
	const char*			file_vv;
	const char*			file_g;
	const char*			file_p;
};

struct rrShaderProgramVsHsDsPs
{
	const char*			file_vv;
	const char*			file_h;
	const char*			file_d;
	const char*			file_p;
};

struct rrShaderProgramVsHsDsGsPs
{
	const char*			file_vv;
	const char*			file_h;
	const char*			file_d;
	const char*			file_g;
	const char*			file_p;
};

class RrShaderProgram : public arBaseObject, public IArResource
{
public:
	// Load shader program. The returned resource will not have its refcount incremented.
	RENDER_API static RrShaderProgram*
							Load ( const rrShaderProgramVsPs& params );
	RENDER_API static RrShaderProgram*
							Load ( const rrShaderProgramVsGsPs& params );
	RENDER_API static RrShaderProgram*
							Load ( const rrShaderProgramVsHsDsPs& params );
	RENDER_API static RrShaderProgram*
							Load ( const rrShaderProgramVsHsDsGsPs& params );

public:
	RENDER_API gpu::ShaderPipeline&
							GetShaderPipeline ( void );

private:
};

#endif//RENDERER_SHADER_PROGRAM_H_