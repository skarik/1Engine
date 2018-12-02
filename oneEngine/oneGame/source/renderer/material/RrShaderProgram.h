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

struct rrShaderProgramCs
{
	const char*			file_cs;
};

class RrShaderProgram : public arBaseObject, public IArResource
{
protected:
	RENDER_API explicit		RrShaderProgram (
		const char* s_resourceId,
		gpu::Shader* vvs,
		gpu::Shader* hs,
		gpu::Shader* ds,
		gpu::Shader* gs,
		gpu::Shader* ps,
		gpu::Shader* cs
		);

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

public: // Resource interface

	//	virtual ResourceType() : What type of resource is this?
	// Identifies the type of resource this is.
	RENDER_API core::arResourceType
							ResourceType ( void ) override { return core::kResourceTypeRrShader; }

	//	virtual ResourceName() : Returns the resource name.
	// This is used to search for the resource. The smaller, the better.
	RENDER_API virtual const char* const
							ResourceName ( void ) override;

public:
	RENDER_API gpu::ShaderPipeline&
							GetShaderPipeline ( void );

private:
	gpu::ShaderPipeline		m_pipeline;
};

#endif//RENDERER_SHADER_PROGRAM_H_