#ifndef RENDERER_SHADER_PROGRAM_H_
#define RENDERER_SHADER_PROGRAM_H_

#include "core/types/arBaseObject.h"
#include "core/containers/arstring.h"
#include "core-ext/resources/IArResource.h"
#include "gpuw/ShaderPipeline.h"

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
	const char*			file_c;
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

	RENDER_API static RrShaderProgram*
							Load ( const rrShaderProgramCs& params );

	struct rrStageToLoad
	{
		gpu::ShaderStage	stage;
		const char*			filename;
	};

protected:
	RENDER_API explicit		RrShaderProgram (
		const char* s_resourceId,
		const rrStageToLoad* stages,
		const int stageCount);

	RENDER_API				~RrShaderProgram ( void );

	//	CreateProgram(...) : Creates a GPUW program with the given shaders.
	RENDER_API void			CreateProgram (
		gpu::Shader* vs,
		gpu::Shader* hs,
		gpu::Shader* ds,
		gpu::Shader* gs,
		gpu::Shader* ps,
		gpu::Shader* cs);

public:
	//	LoadProgramFromDisk(stages[], stageCount) : Loads the given shaders from disk and creates a shader program.
	// Will assert if a shader program is already created.
	RENDER_API void			LoadProgramFromDisk (
		const rrStageToLoad* stages,
		const int stageCount);

	//	FreeProgram() : Frees the currently loaded shader program.
	// Will not assert if no shader program exists.
	RENDER_API void			FreeProgram ( void );

public: // Resource interface

	//	virtual ResourceType() : What type of resource is this?
	// Identifies the type of resource this is.
	RENDER_API core::arResourceType
							ResourceType ( void ) override
		{ return core::kResourceTypeRrShader; }

	//	virtual ResourceName() : Returns the resource name.
	// This is used to search for the resource. The smaller, the better.
	RENDER_API const char* const
							ResourceName ( void ) override
		{ return m_resourceName.c_str(); }

public: // Rendering interface

	//	GetShaderPipeline() : Returns the internal pipeline used for rendering.
	RENDER_API gpu::ShaderPipeline&
							GetShaderPipeline ( void )
		{ return m_pipeline; }

private:
	gpu::ShaderPipeline	m_pipeline;
	arstring256			m_resourceName;

#if BUILD_DEVELOPMENT
public:
	// Store the shader filenames for development and hot-reloading.
	std::string			file_vv;
	std::string			file_h;
	std::string			file_d;
	std::string			file_g;
	std::string			file_p;
	std::string			file_c;
#endif
};

#endif//RENDERER_SHADER_PROGRAM_H_